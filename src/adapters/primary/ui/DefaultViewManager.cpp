#include "DefaultViewManager.hpp"
#include "core/domain/events/UIEvent.hpp"
#include "core/domain/events/core/EventBus.hpp"

DefaultViewManager::DefaultViewManager(std::shared_ptr<DisplayPort> display)
    : display_(display) {}

bool DefaultViewManager::init() {
    if (initialized_) {
        return true;
    }

    // Créer et initialiser toutes les vues
    menuView_ = std::make_shared<MenuView>(display_);
    debugView_ = std::make_shared<DebugView>(display_);
    controlMonitorView_ = std::make_shared<ControlMonitorView>(display_);
    contextualView_ = std::make_shared<ContextualView>(display_);
    modalView_ = std::make_shared<ModalView>(display_);
    splashScreenView_ = std::make_shared<SplashScreenView>(display_);
    lastControlView_ = std::make_shared<LastControlView>(display_);
    
    // Créer PerformanceView et lui passer l'affichage
    performanceView_ = std::make_shared<PerformanceView>(display_);

    // Initialiser chaque vue
    if (!menuView_->init() || !debugView_->init() || !controlMonitorView_->init() ||
        !contextualView_->init() || !modalView_->init() || !splashScreenView_->init() ||
        !lastControlView_->init() || !performanceView_->init()) {
        Serial.println(F("Failed to initialize one or more views"));
        return false;
    }
    
    // Définir le ViewManager pour les vues qui en ont besoin
    menuView_->setViewManager(this);

    // Désactiver explicitement toutes les vues d'abord
    menuView_->setActive(false);
    debugView_->setActive(false);
    controlMonitorView_->setActive(false);
    contextualView_->setActive(false);
    modalView_->setActive(false);
    splashScreenView_->setActive(false);
    lastControlView_->setActive(false);
    performanceView_->setActive(false);

    // Ajouter les vues au vecteur de vues
    views_.push_back(menuView_);
    views_.push_back(debugView_);
    views_.push_back(controlMonitorView_);
    views_.push_back(contextualView_);
    views_.push_back(modalView_);
    views_.push_back(splashScreenView_);
    views_.push_back(lastControlView_);
    views_.push_back(performanceView_);
    
    Serial.println(F("Views initialized and all set to inactive"));
    
    // Par défaut, activer le splash screen en premier
    Serial.println(F("Activating SplashScreen"));
    splashScreenView_->setActive(true);  // Activation directe
    activeView_ = splashScreenView_;

    // Vérifier si la vue modale est active et la masquer explicitement
    if (modalView_ && modalView_->isActive()) {
        Serial.println(F("Explicitly hiding modal dialog at startup"));
        modalView_->setActive(false);
    }

    initialized_ = true;
    
    // Forcer un premier rendu pour montrer le splash screen
    Serial.println(F("Forcing initial render of splash screen"));
    render();
    
    return true;
}

void DefaultViewManager::activateViewExclusively(std::shared_ptr<View> viewToActivate, bool keepModalState) {
    // Si la vue à activer est null, rien à faire
    if (!viewToActivate) {
        Serial.println(F("ERROR: Trying to activate a null view!"));
        return;
    }
    
    // Sauvegarder l'état du modal si nécessaire
    bool modalWasActive = false;
    if (keepModalState && modalView_) {
        modalWasActive = modalView_->isActive();
    }
    
    // Désactiver toutes les vues d'abord
    for (auto& view : views_) {
        view->setActive(false);
    }
    
    // Activer la vue souhaitée
    viewToActivate->setActive(true);
    activeView_ = viewToActivate;
    
    // Restaurer l'état du modal si nécessaire
    if (keepModalState && modalView_) {
        modalView_->setActive(modalWasActive);
    }
    
    // Marquer la vue active et le modal (si actif) comme sales pour forcer le rendu
    viewToActivate->setDirty(true);
    if (modalWasActive) {
        modalView_->setDirty(true);
    }
    
    // Journalisation minimale
    Serial.print(F("View activated: "));
    if (viewToActivate == menuView_) Serial.println(F("Menu"));
    else if (viewToActivate == debugView_) Serial.println(F("Debug"));
    else if (viewToActivate == controlMonitorView_) Serial.println(F("Control Monitor"));
    else if (viewToActivate == contextualView_) Serial.println(F("Contextual"));
    else if (viewToActivate == splashScreenView_) Serial.println(F("Splash Screen"));
    else if (viewToActivate == lastControlView_) Serial.println(F("Last Control"));
    else if (viewToActivate == performanceView_) Serial.println(F("Performance"));
    else Serial.println(F("Unknown"));
}

void DefaultViewManager::checkViewConsistency() {
    if (!initialized_) return;
    
    // Vérifier qu'une seule vue principale est active (hors modal)
    int activeMainViewCount = 0;
    std::shared_ptr<View> lastActiveMainView = nullptr;
    
    for (auto& view : views_) {
        if (view != modalView_ && view->isActive()) {
            activeMainViewCount++;
            lastActiveMainView = view;
        }
    }
    
    // Problème: aucune vue principale active
    if (activeMainViewCount == 0) {
        Serial.println(F("ERROR: No main view active! Activating default view."));
        
        // Activer la vue lastControl par défaut
        bool modalWasActive = modalView_->isActive();
        lastControlView_->setActive(true);
        activeView_ = lastControlView_;
        modalView_->setActive(modalWasActive); // Préserver l'état du modal
    }
    // Problème: plusieurs vues principales actives
    else if (activeMainViewCount > 1) {
        Serial.println(F("ERROR: Multiple main views active! Fixing..."));
        
        // Désactiver toutes les vues principales sauf la dernière qui a été activée
        bool modalWasActive = modalView_->isActive();
        
        for (auto& view : views_) {
            if (view != modalView_ && view != lastActiveMainView) {
                view->setActive(false);
            }
        }
        
        // Assurer que la dernière vue active est bien active
        lastActiveMainView->setActive(true);
        activeView_ = lastActiveMainView;
        modalView_->setActive(modalWasActive); // Préserver l'état du modal
        
        Serial.println(F("View consistency fixed."));
    }
    
    // Vérifier si activeView_ est cohérent avec les vues actives
    if (activeView_ && !activeView_->isActive()) {
        Serial.println(F("ERROR: activeView_ is not active! Fixing..."));
        activeView_->setActive(true);
    }
}

void DefaultViewManager::update() {
    if (!initialized_) return;

    // Débogage: imprimer l'état des vues périodiquement
    static unsigned long lastDebugTime = 0;
    unsigned long currentTime = millis();
    
    if (currentTime - lastDebugTime >= 5000) {
        lastDebugTime = currentTime;
        // Auto-vérification de l'état des vues
        checkViewConsistency();
    }

    // Gérer la transition du splash screen à la vue par défaut
    if (splashScreenView_->isActive() && splashScreenView_->isSplashScreenCompleted()) {
        // Serial.println(F("Splash screen completed, switching to LastControlView"));
        activateViewExclusively(lastControlView_, false);
    }

    // Mettre à jour toutes les vues actives
    for (auto& view : views_) {
        if (view->isActive()) {
            view->update();
        }
    }
    
    // Vérifier si une vue active a changé et nécessite un rendu
    bool needsRedraw = false;
    
    if (activeView_ && activeView_->isDirty()) {
        needsRedraw = true;
    }
    
    // Vérifier l'état de saleté des overlays
    if (modalView_->isActive() && modalView_->isDirty()) {
        needsRedraw = true;
    }
    
    if (lastControlView_->isActive() && lastControlView_->isDirty()) {
        needsRedraw = true;
    }
    
    // Redessiner uniquement si nécessaire
    if (needsRedraw) {
        render();
        
        // Après le rendu, marquer les vues comme propres
        if (activeView_) activeView_->setDirty(false);
        if (modalView_->isActive()) modalView_->setDirty(false);
        if (lastControlView_->isActive()) lastControlView_->setDirty(false);
    }
}

void DefaultViewManager::render() {
    if (!initialized_) return;

    // Effacer complètement l'écran avant de rendre
    display_->clear();
    
    // Vérifier et rendre la vue principale active
    if (activeView_ && activeView_->isActive()) {
        activeView_->render();
    } else {
        // Si aucune vue n'est active, c'est un état problématique - activer la vue par défaut
        static bool warningShown = false;
        if (!warningShown) {
            Serial.println(F("WARNING: No active view to render! Activating default view."));
            warningShown = true;
            
            // Activer la vue LastControl par défaut
            lastControlView_->setActive(true);
            activeView_ = lastControlView_;
            activeView_->render();
        }
    }
    
    // Rendre la vue modale par-dessus si elle est active
    if (modalView_ && modalView_->isActive()) {
        modalView_->render();
    }

    // Au lieu de mettre à jour l'affichage directement, émettre un événement
    // pour demander une mise à jour asynchrone
    auto event = std::make_shared<DisplayUpdateRequestedEvent>();
    EventBus::getInstance().publish(*event);
    
    // Forcer une demande immédiate de mise à jour pour éviter les problèmes de timing
    // Cette ligne est critique pour assurer le bon fonctionnement - si nécessaire en production
    static int forceUpdateCounter = 0;
    if (++forceUpdateCounter % 5 == 0) { // 1 fois sur 5 rend plus fiable
        // Mettre à jour l'affichage directement comme solution de secours 
        // pour s'assurer que l'écran se met bien à jour
        display_->update();
    }
    
    // Compter le nombre de rendus pour débuguer
    static unsigned long lastRenderCountTime = 0;
    static int renderCount = 0;
    
    renderCount++;
    
    unsigned long currentTime = millis();
    if (currentTime - lastRenderCountTime >= 10000) { // Afficher tous les 10 secondes
        Serial.print(F("Render count in last 10s: "));
        Serial.println(renderCount);
        renderCount = 0;
        lastRenderCountTime = currentTime;
    }
}

void DefaultViewManager::enterMenu() {
    if (!initialized_) return;
    activateViewExclusively(menuView_);
}

void DefaultViewManager::exitMenu() {
    if (!initialized_) return;
    activateViewExclusively(lastControlView_);
}

void DefaultViewManager::selectNextMenuItem() {
    if (!initialized_ || !menuView_->isActive()) return;
    menuView_->selectNext();
}

void DefaultViewManager::selectPreviousMenuItem() {
    if (!initialized_ || !menuView_->isActive()) return;
    menuView_->selectPrevious();
}

void DefaultViewManager::selectMenuItem(int index) {
    if (!initialized_ || !menuView_->isActive()) return;
    menuView_->selectItem(index);
}

int DefaultViewManager::getCurrentMenuIndex() const {
    if (!initialized_ || !menuView_->isActive()) return -1;
    return menuView_->getCurrentIndex();
}

bool DefaultViewManager::isInMenu() const {
    if (!initialized_) return false;
    return menuView_->isActive();
}

void DefaultViewManager::showMainScreen() {
    if (!initialized_) return;
    activateViewExclusively(contextualView_);
}

void DefaultViewManager::showControlMonitor() {
    if (!initialized_) return;
    activateViewExclusively(controlMonitorView_);
}

void DefaultViewManager::showDebugScreen() {
    if (!initialized_) return;
    activateViewExclusively(debugView_);
}

void DefaultViewManager::showLastControlView() {
    if (!initialized_) return;
    
    Serial.println(F("DefaultViewManager::showLastControlView() - Activating LastControlView"));
    
    // Vérifier si lastControlView_ est valide
    if (!lastControlView_) {
        Serial.println(F("ERROR: lastControlView_ is null!"));
        return;
    }
    
    activateViewExclusively(lastControlView_);
    
    // Forcer un rendu immédiat
    render();
}

void DefaultViewManager::showPerformanceView() {
    if (!initialized_) return;
    
    Serial.println(F("DefaultViewManager::showPerformanceView() - Activating PerformanceView"));
    
    // Vérifier si performanceView_ est valide
    if (!performanceView_) {
        Serial.println(F("ERROR: performanceView_ is null!"));
        return;
    }
    
    activateViewExclusively(performanceView_);
    
    // Forcer un rendu immédiat
    render();
}

void DefaultViewManager::setActiveView(ViewType type) {
    if (!initialized_) return;
    
    Serial.print(F("DefaultViewManager::setActiveView() - Activating view type: "));
    Serial.println(static_cast<int>(type));
    
    switch (type) {
        case ViewType::Menu:
            activateViewExclusively(menuView_);
            break;
        case ViewType::ControlMonitor:
            activateViewExclusively(controlMonitorView_);
            break;
        case ViewType::Debug:
            activateViewExclusively(debugView_);
            break;
        case ViewType::ContextualView:
            activateViewExclusively(contextualView_);
            break;
        case ViewType::SplashScreen:
            activateViewExclusively(splashScreenView_);
            break;
        case ViewType::LastControl:
            activateViewExclusively(lastControlView_);
            break;
        case ViewType::Performance:
            activateViewExclusively(performanceView_);
            break;
        case ViewType::Modal:
            // On ne peut pas activer uniquement la vue modale
            // mais on peut la rendre visible au-dessus de la vue actuelle
            if (modalView_) {
                modalView_->setActive(true);
            }
            break;
        default:
            Serial.println(F("WARNING: Unknown view type"));
            break;
    }
    
    // Forcer un rendu immédiat
    render();
}

void DefaultViewManager::showModalDialog(const String& message) {
    if (!initialized_) return;

    modalView_->setMessage(message.c_str());
    modalView_->setActive(true);
    modalView_->setDirty(true);
}

void DefaultViewManager::hideModalDialog() {
    if (!initialized_) return;
    modalView_->setActive(false);
    modalView_->setDirty(true);
}

void DefaultViewManager::toggleModalDialogButton() {
    if (!initialized_ || !modalView_->isActive()) return;
    modalView_->toggleButton();
    modalView_->setDirty(true);
}

bool DefaultViewManager::isModalDialogOkSelected() const {
    if (!initialized_ || !modalView_->isActive()) return true;
    return modalView_->isOkSelected();
}

void DefaultViewManager::scrollMainScreenByDelta(int8_t delta) {
    if (!initialized_ || !contextualView_->isActive()) return;
    contextualView_->scroll(delta);
}

void DefaultViewManager::scrollControlMonitorByDelta(int8_t delta) {
    if (!initialized_ || !controlMonitorView_->isActive()) return;
    controlMonitorView_->scroll(delta);
}

void DefaultViewManager::scrollDebugLogByDelta(int8_t delta) {
    if (!initialized_ || !debugView_->isActive()) return;
    debugView_->scroll(delta);
}

void DefaultViewManager::updateControlMonitorInfo(uint8_t controlId, const String& type,
                                                uint8_t channel, uint8_t number, uint8_t value) {
    if (!initialized_) return;
    
    // Log de débogage
    Serial.print(F("\nDefaultViewManager: Updating control info for controlId="));
    Serial.print(controlId);
    Serial.print(F(" type="));
    Serial.print(type);
    Serial.print(F(" channel="));
    Serial.print(channel);
    Serial.print(F(" number="));
    Serial.print(number);
    Serial.print(F(" value="));
    Serial.println(value);
    
    // Mettre à jour les informations sur le moniteur de contrôle
    controlMonitorView_->updateControlInfo(controlId, type, channel, number, value);
    
    // Mettre à jour les informations sur la vue du dernier contrôle
    if (lastControlView_) {
        lastControlView_->updateLastControl(controlId, type, channel, number, value);
        Serial.println(F("LastControlView updated"));
        
        // Activer LastControlView si nous ne sommes pas dans d'autres vues spécifiques
        if (!menuView_->isActive() && !debugView_->isActive() && 
            !controlMonitorView_->isActive()) {
            
            Serial.println(F("Automatically switching to LastControlView after control update"));
            
            // Désactiver toutes les vues principales
            menuView_->setActive(false);
            debugView_->setActive(false);
            controlMonitorView_->setActive(false);
            contextualView_->setActive(false);
            splashScreenView_->setActive(false);
            
            // Activer LastControlView et le définir comme vue active
            lastControlView_->setActive(true);
            activeView_ = lastControlView_;
            
            // Forcer un rendu immédiat pour afficher les changements
            render();
        }
    } else {
        Serial.println(F("ERROR: lastControlView_ is null!"));
    }
}

void DefaultViewManager::updateEncoderPosition(uint8_t encoderId, int32_t position) {
    if (!initialized_) return;
    
    // Mettre à jour les vues qui ont besoin de cette information
    contextualView_->updateEncoderPosition(encoderId, position);
    controlMonitorView_->updateEncoderPosition(encoderId, position);
}

void DefaultViewManager::updateEncoderButtonState(uint8_t encoderId, bool pressed) {
    if (!initialized_) return;
    
    // Mettre à jour les vues qui ont besoin de cette information
    contextualView_->updateEncoderButtonState(encoderId, pressed);
    controlMonitorView_->updateEncoderButtonState(encoderId, pressed);
}

void DefaultViewManager::updateButtonState(uint8_t buttonId, bool pressed) {
    if (!initialized_) return;
    
    // Mettre à jour les vues qui ont besoin de cette information
    contextualView_->updateButtonState(buttonId, pressed);
    controlMonitorView_->updateButtonState(buttonId, pressed);
}