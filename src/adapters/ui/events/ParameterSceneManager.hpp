#pragma once

#include <lvgl.h>

#include <array>
#include <functional>
#include <memory>

#include "ParameterWidgetMappingManager.hpp"
#include "adapters/ui/components/ButtonIndicator.hpp"
#include "adapters/ui/components/ParameterWidget.hpp"

/**
 * @brief Gestionnaire de scène LVGL pour les widgets de paramètres
 * 
 * Cette classe centralise la gestion des objets LVGL pour une vue de paramètres :
 * - Création et configuration de l'écran principal
 * - Gestion du container grille 4x2
 * - Création et positionnement des ParameterWidget
 * - Configuration des indicateurs de boutons
 * - Nettoyage des ressources LVGL
 * 
 * Respecte le principe de responsabilité unique en se concentrant uniquement
 * sur la gestion des objets LVGL et leur hiérarchie.
 */
class ParameterSceneManager {
public:
    /**
     * @brief Configuration pour le gestionnaire de scène
     */
    struct SceneConfig {
        uint8_t maxWidgets;          // Nombre maximum de widgets (8 par défaut)
        uint16_t screenWidth;        // Largeur de l'écran (320 par défaut)
        uint16_t screenHeight;       // Hauteur de l'écran (240 par défaut)
        uint8_t gridCols;            // Colonnes de la grille (4 par défaut)
        uint8_t gridRows;            // Lignes de la grille (2 par défaut)
        uint16_t widgetWidth;        // Largeur d'un widget (80 par défaut)
        uint16_t widgetHeight;       // Hauteur d'un widget (120 par défaut)
        uint16_t arcRadius;          // Rayon de l'arc des widgets (70 par défaut)
        uint8_t buttonIndicatorSize; // Taille des indicateurs de bouton (12 par défaut)
        uint32_t backgroundColor;    // Couleur de fond (0x000000 par défaut)
        bool enableLogging;          // Activer le logging de debug
        
        SceneConfig() : maxWidgets(8), screenWidth(320), screenHeight(240),
                       gridCols(4), gridRows(2), widgetWidth(80), widgetHeight(120),
                       arcRadius(70), buttonIndicatorSize(12), backgroundColor(0x000000),
                       enableLogging(false) {}
    };

    /**
     * @brief Callback pour accéder à la configuration des widgets
     * @param index Index du widget (0-7)
     * @return Informations de configuration ou nullptr si invalide
     */
    struct WidgetConfig {
        uint8_t cc_number;
        uint8_t channel;
        uint8_t value;
        String name;
        bool visible;
    };
    using WidgetConfigAccessor = std::function<WidgetConfig*(uint8_t index)>;

    /**
     * @brief Constructeur avec configuration
     * @param config Configuration de la scène
     * @param mappingManager Gestionnaire de mappings pour les boutons (optionnel)
     */
    explicit ParameterSceneManager(
        const SceneConfig& config,
        std::shared_ptr<ParameterWidgetMappingManager> mappingManager = nullptr);

    /**
     * @brief Destructeur - nettoie automatiquement les objets LVGL
     */
    ~ParameterSceneManager();

    /**
     * @brief Initialise la scène LVGL complète
     * @param widgetConfigAccessor Fonction pour obtenir la configuration des widgets
     * @return true si l'initialisation réussit
     */
    bool initializeScene(WidgetConfigAccessor widgetConfigAccessor = nullptr);

    /**
     * @brief Nettoie tous les objets LVGL créés
     */
    void cleanup();

    /**
     * @brief Vérifie si la scène est initialisée
     * @return true si initialisée
     */
    bool isInitialized() const;

    /**
     * @brief Met à jour la visibilité de tous les widgets
     * @param visible État de visibilité
     */
    void setWidgetsVisible(bool visible);

    /**
     * @brief Finalise le positionnement après l'initialisation LVGL
     * 
     * Cette méthode doit être appelée après que LVGL ait terminé
     * son processus d'initialisation pour garantir un positionnement correct.
     */
    void finalizePositioning();

    /**
     * @brief Met à jour le gestionnaire de mappings
     * @param mappingManager Nouveau gestionnaire de mappings
     */
    void updateMappingManager(std::shared_ptr<ParameterWidgetMappingManager> mappingManager);

    // === ACCESSEURS ===

    /**
     * @brief Obtient l'écran principal LVGL
     * @return Pointeur vers l'écran ou nullptr si non initialisé
     */
    lv_obj_t* getMainScreen() const;

    /**
     * @brief Obtient le container grille
     * @return Pointeur vers le container ou nullptr si non initialisé
     */
    lv_obj_t* getGridContainer() const;

    /**
     * @brief Obtient un widget par index
     * @param index Index du widget (0-7)
     * @return Pointeur vers le widget ou nullptr si invalide
     */
    ParameterWidget* getWidget(uint8_t index) const;

    /**
     * @brief Obtient tous les widgets
     * @return Référence constante vers le tableau de widgets
     */
    const std::array<std::unique_ptr<ParameterWidget>, 8>& getWidgets() const;

    /**
     * @brief Obtient le nombre de widgets créés
     * @return Nombre de widgets actifs
     */
    uint8_t getWidgetCount() const;

    /**
     * @brief Obtient les statistiques de la scène
     */
    struct SceneStats {
        uint8_t widgetsCreated;
        uint8_t buttonIndicatorsCreated;
        bool sceneInitialized;
        uint32_t memoryUsageEstimate; // Estimation en bytes
    };
    SceneStats getStats() const;

private:
    SceneConfig config_;
    std::shared_ptr<ParameterWidgetMappingManager> mappingManager_;
    bool initialized_;
    
    // Objets LVGL
    lv_obj_t* main_screen_;
    lv_obj_t* grid_container_;
    std::array<std::unique_ptr<ParameterWidget>, 8> parameter_widgets_;
    
    // Statistiques
    mutable SceneStats stats_;

    /**
     * @brief Crée et configure l'écran principal
     * @return true si succès
     */
    bool createMainScreen();

    /**
     * @brief Crée et configure le container grille
     * @return true si succès
     */
    bool createGridContainer();

    /**
     * @brief Crée tous les widgets de paramètres
     * @param widgetConfigAccessor Fonction pour obtenir la configuration
     * @return true si succès
     */
    bool createParameterWidgets(WidgetConfigAccessor widgetConfigAccessor);

    /**
     * @brief Configure les indicateurs de boutons sur les widgets
     */
    void setupButtonIndicators();

    /**
     * @brief Finalise le positionnement des indicateurs de boutons
     */
    void finalizeButtonIndicatorPositions();

    /**
     * @brief Nettoie l'écran principal
     */
    void cleanupMainScreen();

    /**
     * @brief Nettoie le container grille
     */
    void cleanupGridContainer();

    /**
     * @brief Nettoie tous les widgets
     */
    void cleanupParameterWidgets();

    /**
     * @brief Met à jour les statistiques
     */
    void updateStats() const;

    /**
     * @brief Log une information si le logging est activé
     * @param message Message à logger
     */
    void logInfo(const String& message) const;

    /**
     * @brief Log un debug si le logging est activé
     * @param message Message de debug à logger
     */
    void logDebug(const String& message) const;

    /**
     * @brief Log une erreur si le logging est activé
     * @param message Message d'erreur à logger
     */
    void logError(const String& message) const;
};