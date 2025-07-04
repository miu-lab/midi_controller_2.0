#include "LvglSceneManager.hpp"
#include <Arduino.h>

LvglSceneManager::LvglSceneManager(const SceneConfig& config,
                                 std::shared_ptr<WidgetMappingManager> mappingManager)
    : config_(config)
    , mappingManager_(mappingManager)
    , initialized_(false)
    , main_screen_(nullptr)
    , grid_container_(nullptr)
    , stats_{0, 0, false, 0} {
    
    logInfo("LvglSceneManager initialized with " + String(config_.maxWidgets) + " max widgets");
}

LvglSceneManager::~LvglSceneManager() {
    cleanup();
    logInfo("LvglSceneManager destroyed");
}

bool LvglSceneManager::initializeScene(WidgetConfigAccessor widgetConfigAccessor) {
    if (initialized_) {
        logDebug("Scene already initialized");
        return true;
    }
    
    logInfo("Initializing LVGL scene...");
    
    // Créer l'écran principal
    if (!createMainScreen()) {
        logError("Failed to create main screen");
        return false;
    }
    
    // Créer le container grille
    if (!createGridContainer()) {
        logError("Failed to create grid container");
        cleanup();
        return false;
    }
    
    // Créer les widgets de paramètres
    if (!createParameterWidgets(widgetConfigAccessor)) {
        logError("Failed to create parameter widgets");
        cleanup();
        return false;
    }
    
    // Configurer les indicateurs de boutons
    setupButtonIndicators();
    
    initialized_ = true;
    updateStats();
    
    logInfo("LVGL scene initialized successfully");
    return true;
}

void LvglSceneManager::cleanup() {
    if (!initialized_ && !main_screen_ && !grid_container_) {
        return; // Déjà nettoyé
    }
    
    logDebug("Cleaning up LVGL scene...");
    
    // Nettoyer dans l'ordre inverse de création
    cleanupParameterWidgets();
    cleanupGridContainer();
    cleanupMainScreen();
    
    initialized_ = false;
    updateStats();
    
    logDebug("LVGL scene cleanup completed");
}

bool LvglSceneManager::isInitialized() const {
    return initialized_;
}

void LvglSceneManager::setWidgetsVisible(bool visible) {
    for (auto& widget : parameter_widgets_) {
        if (widget) {
            widget->setVisible(visible);
        }
    }
    
    logDebug("Set all widgets visible: " + String(visible ? "true" : "false"));
}

void LvglSceneManager::finalizePositioning() {
    finalizeButtonIndicatorPositions();
    logDebug("Finalized positioning for all elements");
}

void LvglSceneManager::updateMappingManager(std::shared_ptr<WidgetMappingManager> mappingManager) {
    mappingManager_ = mappingManager;
    
    // Reconfigurer les indicateurs de boutons si nécessaire
    if (initialized_) {
        setupButtonIndicators();
        finalizeButtonIndicatorPositions();
    }
    
    logInfo("Updated mapping manager");
}

//=============================================================================
// Accesseurs
//=============================================================================

lv_obj_t* LvglSceneManager::getMainScreen() const {
    return main_screen_;
}

lv_obj_t* LvglSceneManager::getGridContainer() const {
    return grid_container_;
}

ParameterWidget* LvglSceneManager::getWidget(uint8_t index) const {
    if (index < config_.maxWidgets && parameter_widgets_[index]) {
        return parameter_widgets_[index].get();
    }
    return nullptr;
}

const std::array<std::unique_ptr<ParameterWidget>, 8>& LvglSceneManager::getWidgets() const {
    return parameter_widgets_;
}

uint8_t LvglSceneManager::getWidgetCount() const {
    uint8_t count = 0;
    for (const auto& widget : parameter_widgets_) {
        if (widget) {
            count++;
        }
    }
    return count;
}

LvglSceneManager::SceneStats LvglSceneManager::getStats() const {
    updateStats();
    return stats_;
}

//=============================================================================
// Méthodes privées de création
//=============================================================================

bool LvglSceneManager::createMainScreen() {
    logDebug("Creating main screen...");
    
    // Créer l'écran principal
    main_screen_ = lv_obj_create(nullptr);
    if (!main_screen_) {
        logError("Failed to create main screen object");
        return false;
    }

    // Style de l'écran - fond sombre
    lv_obj_set_style_bg_color(main_screen_, lv_color_hex(config_.backgroundColor), 0);
    lv_obj_set_style_bg_opa(main_screen_, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(main_screen_, 0, 0);

    logDebug("Main screen created successfully");
    return true;
}

bool LvglSceneManager::createGridContainer() {
    if (!main_screen_) {
        logError("Main screen not available for grid container creation");
        return false;
    }

    logDebug("Creating grid container...");

    // Créer le container principal pour la grille
    grid_container_ = lv_obj_create(main_screen_);
    if (!grid_container_) {
        logError("Failed to create grid container object");
        return false;
    }
    
    // Configuration du container pour une grille configurable
    lv_obj_set_size(grid_container_, config_.screenWidth, config_.screenHeight);
    lv_obj_set_pos(grid_container_, 0, 0);
    
    // Style du container
    lv_obj_set_style_bg_opa(grid_container_, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(grid_container_, 0, 0);
    lv_obj_set_style_pad_all(grid_container_, 0, 0);
    lv_obj_set_style_pad_gap(grid_container_, 0, 0);
    
    // Configurer le layout en grille dynamique
    static lv_coord_t col_dsc[5]; // Max 4 colonnes + terminateur
    static lv_coord_t row_dsc[3]; // Max 2 lignes + terminateur
    
    // Calculer les largeurs de colonnes
    lv_coord_t col_width = config_.screenWidth / config_.gridCols;
    for (uint8_t i = 0; i < config_.gridCols; i++) {
        col_dsc[i] = col_width;
    }
    col_dsc[config_.gridCols] = LV_GRID_TEMPLATE_LAST;
    
    // Calculer les hauteurs de lignes
    lv_coord_t row_height = config_.screenHeight / config_.gridRows;
    for (uint8_t i = 0; i < config_.gridRows; i++) {
        row_dsc[i] = row_height;
    }
    row_dsc[config_.gridRows] = LV_GRID_TEMPLATE_LAST;
    
    lv_obj_set_grid_dsc_array(grid_container_, col_dsc, row_dsc);
    lv_obj_set_layout(grid_container_, LV_LAYOUT_GRID);
    
    logDebug("Grid container created with " + String(config_.gridCols) + "x" + String(config_.gridRows) + " layout");
    return true;
}

bool LvglSceneManager::createParameterWidgets(WidgetConfigAccessor widgetConfigAccessor) {
    if (!grid_container_) {
        logError("Grid container not available for widget creation");
        return false;
    }

    logDebug("Creating " + String(config_.maxWidgets) + " parameter widgets...");

    // Créer les widgets selon la configuration
    for (uint8_t i = 0; i < config_.maxWidgets; i++) {
        // Créer le widget avec dimensions configurées
        parameter_widgets_[i] = std::make_unique<ParameterWidget>(
            grid_container_, 
            config_.widgetWidth, 
            config_.widgetHeight, 
            config_.arcRadius
        );
        
        if (!parameter_widgets_[i]) {
            logError("Failed to create parameter widget " + String(i));
            return false;
        }
        
        // Calculer position dans la grille
        uint8_t col = i % config_.gridCols;
        uint8_t row = i / config_.gridCols;
        
        // Positionner dans la grille
        lv_obj_set_grid_cell(parameter_widgets_[i]->getContainer(), 
                             LV_GRID_ALIGN_CENTER, col, 1,
                             LV_GRID_ALIGN_CENTER, row, 1);
        
        // Configurer le widget si un accesseur est fourni
        if (widgetConfigAccessor) {
            WidgetConfig* config = widgetConfigAccessor(i);
            if (config) {
                parameter_widgets_[i]->setParameter(
                    config->cc_number, 
                    config->channel, 
                    config->value, 
                    config->name, 
                    false // Pas d'animation lors de l'initialisation
                );
                parameter_widgets_[i]->setVisible(config->visible);
            }
        } else {
            // Configuration par défaut
            parameter_widgets_[i]->setParameter(i + 1, 1, 0, "CC" + String(i + 1), false);
        }
    }

    logDebug("Created " + String(config_.maxWidgets) + " parameter widgets successfully");
    return true;
}

void LvglSceneManager::setupButtonIndicators() {
    if (!mappingManager_) {
        logDebug("No mapping manager available for button indicators");
        return;
    }
    
    logDebug("Setting up button indicators...");
    
    // Ajouter des indicateurs aux widgets qui ont des boutons enfants
    const auto& buttonMapping = mappingManager_->getButtonToWidgetMapping();
    uint8_t indicatorsCreated = 0;
    
    for (const auto& mapping : buttonMapping) {
        uint8_t widget_index = mapping.second;
        
        if (widget_index < config_.maxWidgets && parameter_widgets_[widget_index]) {
            // Ajouter un indicateur de bouton à ce widget
            parameter_widgets_[widget_index]->addButtonIndicator(config_.buttonIndicatorSize);
            indicatorsCreated++;
        }
    }
    
    logDebug("Created " + String(indicatorsCreated) + " button indicators");
}

void LvglSceneManager::finalizeButtonIndicatorPositions() {
    if (!mappingManager_) {
        return;
    }
    
    logDebug("Finalizing button indicator positions...");
    
    // Ré-appliquer les positions et tailles après que LVGL ait fini l'initialisation
    const auto& buttonMapping = mappingManager_->getButtonToWidgetMapping();
    uint8_t positionsFinalized = 0;
    
    for (const auto& mapping : buttonMapping) {
        uint8_t widget_index = mapping.second;
        
        if (widget_index < config_.maxWidgets && parameter_widgets_[widget_index]) {
            ParameterWidget* widget = parameter_widgets_[widget_index].get();
            if (widget && widget->hasButtonIndicator()) {
                ButtonIndicator* indicator = widget->getButtonIndicator();
                if (indicator && indicator->getLedObject()) {
                    lv_obj_t* led = indicator->getLedObject();
                    
                    // Forcer la taille à nouveau
                    lv_obj_set_size(led, config_.buttonIndicatorSize, config_.buttonIndicatorSize);
                    
                    // Centrer la LED sur son parent (arc)
                    lv_obj_center(led);
                    
                    // Forcer au premier plan
                    lv_obj_move_foreground(led);
                    
                    // Invalider pour redraw
                    lv_obj_invalidate(led);
                    
                    positionsFinalized++;
                }
            }
        }
    }
    
    logDebug("Finalized " + String(positionsFinalized) + " button indicator positions");
}

//=============================================================================
// Méthodes privées de nettoyage
//=============================================================================

void LvglSceneManager::cleanupParameterWidgets() {
    logDebug("Cleaning up parameter widgets...");
    
    // Nettoyer tous les widgets en premier
    for (auto& widget : parameter_widgets_) {
        if (widget) {
            widget.reset();
        }
    }
    
    logDebug("Parameter widgets cleaned up");
}

void LvglSceneManager::cleanupGridContainer() {
    if (grid_container_) {
        logDebug("Cleaning up grid container...");
        lv_obj_delete(grid_container_);
        grid_container_ = nullptr;
        logDebug("Grid container cleaned up");
    }
}

void LvglSceneManager::cleanupMainScreen() {
    if (main_screen_) {
        logDebug("Cleaning up main screen...");
        lv_obj_delete(main_screen_);
        main_screen_ = nullptr;
        logDebug("Main screen cleaned up");
    }
}

//=============================================================================
// Utilitaires
//=============================================================================

void LvglSceneManager::updateStats() const {
    stats_.widgetsCreated = getWidgetCount();
    stats_.sceneInitialized = initialized_;
    
    // Compter les indicateurs de boutons
    stats_.buttonIndicatorsCreated = 0;
    for (const auto& widget : parameter_widgets_) {
        if (widget && widget->hasButtonIndicator()) {
            stats_.buttonIndicatorsCreated++;
        }
    }
    
    // Estimation grossière de l'utilisation mémoire
    stats_.memoryUsageEstimate = 
        sizeof(LvglSceneManager) +
        (stats_.widgetsCreated * sizeof(ParameterWidget)) +
        (stats_.buttonIndicatorsCreated * sizeof(ButtonIndicator)) +
        (main_screen_ ? 200 : 0) +  // Estimation pour objets LVGL
        (grid_container_ ? 150 : 0);
}

void LvglSceneManager::logInfo(const String& message) const {
    if (config_.enableLogging) {
        Serial.println("[LvglSceneManager] " + message);
    }
}

void LvglSceneManager::logDebug(const String& message) const {
    if (config_.enableLogging) {
        Serial.println("[LvglSceneManager DEBUG] " + message);
    }
}

void LvglSceneManager::logError(const String& message) const {
    if (config_.enableLogging) {
        Serial.println("[LvglSceneManager ERROR] " + message);
    }
}