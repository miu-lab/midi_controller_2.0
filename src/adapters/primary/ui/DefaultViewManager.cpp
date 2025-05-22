#include "DefaultViewManager.hpp"

DefaultViewManager::DefaultViewManager(std::shared_ptr<DisplayPort> display)
    : display_(display) {}

bool DefaultViewManager::init() {
    if (initialized_) {
        return true;
    }

    Serial.println(F("DefaultViewManager: Initializing simplified view system..."));

    // Créer les 4 vues uniquement
    splashView_ = std::make_shared<SplashScreenView>(display_);
    parameterView_ = std::make_shared<ParameterFocusView>(display_);
    menuView_ = std::make_shared<MenuView>(display_);
    modalView_ = std::make_shared<ModalView>(display_);

    // Initialiser chaque vue
    if (!splashView_->init() || !parameterView_->init() || 
        !menuView_->init() || !modalView_->init()) {
        Serial.println(F("DefaultViewManager: Failed to initialize views"));
        return false;
    }

    // Définir le ViewManager pour les vues qui en ont besoin
    menuView_->setViewManager(this);

    // Désactiver toutes les vues d'abord
    deactivateAllViews();

    Serial.println(F("DefaultViewManager: Starting with SplashScreen"));
    
    // Activer le splash screen
    currentView_ = ViewType::SplashScreen;
    splashView_->setActive(true);

    initialized_ = true;
    
    // Forcer un premier rendu immédiat
    Serial.println(F("Force initial render..."));
    render();
    
    return true;
}

void DefaultViewManager::update() {
    if (!initialized_) {
        Serial.println(F("DefaultViewManager::update() - Not initialized!"));
        return;
    }

    // Gérer la transition automatique du splash screen
    if (currentView_ == ViewType::SplashScreen) {
        Serial.println(F("Checking splash screen completion..."));
        if (splashView_ && splashView_->isSplashScreenCompleted()) {
            Serial.println(F("DefaultViewManager: Splash completed, switching to ParameterFocus"));
            showHome(); // Vue par défaut
        }
    }

    // Mettre à jour la vue active
    switch (currentView_) {
        case ViewType::SplashScreen:
            if (splashView_) {
                splashView_->update();
            }
            break;
        case ViewType::ParameterFocus:
            if (parameterView_) {
                parameterView_->update();
            }
            break;
        case ViewType::Menu:
            if (menuView_) {
                menuView_->update();
            }
            break;
        default:
            break;
    }

    // Toujours mettre à jour le modal s'il est actif
    if (modalView_ && modalView_->isActive()) {
        modalView_->update();
    }
}

void DefaultViewManager::render() {
    if (!initialized_) {
        Serial.println(F("DefaultViewManager::render() - Not initialized!"));
        return;
    }

    Serial.print(F("DefaultViewManager::render() - Current view: "));
    Serial.println(static_cast<int>(currentView_));

    display_->clear();

    // Rendre la vue active
    switch (currentView_) {
        case ViewType::SplashScreen:
            Serial.println(F("Rendering SplashScreen"));
            if (splashView_ && splashView_->isActive()) {
                splashView_->render();
                Serial.println(F("SplashScreen rendered"));
            } else {
                Serial.println(F("SplashScreen not active or null!"));
            }
            break;
        case ViewType::ParameterFocus:
            Serial.println(F("Rendering ParameterFocus"));
            if (parameterView_ && parameterView_->isActive()) {
                parameterView_->render();
            } else {
                Serial.println(F("ParameterFocus not active or null!"));
            }
            break;
        case ViewType::Menu:
            Serial.println(F("Rendering Menu"));
            if (menuView_ && menuView_->isActive()) {
                menuView_->render();
            } else {
                Serial.println(F("Menu not active or null!"));
            }
            break;
        default:
            Serial.println(F("Unknown view type - falling back to ParameterFocus"));
            // Vue par défaut si problème
            if (parameterView_ && parameterView_->isActive()) {
                parameterView_->render();
            }
            break;
    }

    // Rendre le modal par-dessus si actif
    if (modalView_ && modalView_->isActive()) {
        Serial.println(F("Rendering Modal"));
        modalView_->render();
    }

    Serial.println(F("Calling display_->update()"));
    display_->update();
    Serial.println(F("Display updated"));
}

void DefaultViewManager::showParameterFocus(uint8_t ccNumber, uint8_t channel, uint8_t value, const String& parameterName) {
    if (!initialized_) return;

    Serial.print(F("DefaultViewManager: Showing ParameterFocus - CC"));
    Serial.print(ccNumber);
    Serial.print(F(" CH"));
    Serial.print(channel);
    Serial.print(F(" = "));
    Serial.println(value);

    // Mettre à jour les informations du paramètre
    parameterView_->setParameter(ccNumber, channel, value, parameterName);
    
    // Activer la vue
    activateView(ViewType::ParameterFocus);
    
    // Forcer un rendu immédiat après activation
    Serial.println(F("Forcing render after ParameterFocus activation"));
    render();
}

void DefaultViewManager::updateParameterValue(uint8_t value) {
    if (!initialized_ || currentView_ != ViewType::ParameterFocus) return;
    
    parameterView_->setValue(value);
}

void DefaultViewManager::showMenu() {
    if (!initialized_) return;

    Serial.println(F("DefaultViewManager: Showing Menu"));
    activateView(ViewType::Menu);
}

void DefaultViewManager::showHome() {
    if (!initialized_) return;

    Serial.println(F("DefaultViewManager: Showing Home (ParameterFocus default)"));
    
    // Si pas de paramètre actuel, afficher un paramètre par défaut
    if (currentView_ != ViewType::ParameterFocus) {
        Serial.println(F("Setting default parameter for ParameterFocus"));
        parameterView_->setParameter(0, 1, 0, "READY");
    }
    
    activateView(ViewType::ParameterFocus);
}

void DefaultViewManager::showModal(const String& message) {
    if (!initialized_) return;

    modalView_->setMessage(message.c_str());
    modalView_->setActive(true);
}

void DefaultViewManager::hideModal() {
    if (!initialized_) return;
    
    modalView_->setActive(false);
}

void DefaultViewManager::navigateMenu(int8_t direction) {
    if (!initialized_ || currentView_ != ViewType::Menu) return;
    
    if (direction > 0) {
        menuView_->selectNext();
    } else if (direction < 0) {
        menuView_->selectPrevious();
    }
}

void DefaultViewManager::selectMenuItem() {
    if (!initialized_ || currentView_ != ViewType::Menu) return;
    
    // Laisser le MenuView gérer la sélection
    // Il appellera les méthodes appropriées via le ViewManager
    int selectedIndex = menuView_->getCurrentIndex();
    
    // Actions basées sur l'élément sélectionné
    switch (selectedIndex) {
        case 0: // Premier élément - retour home
            showHome();
            break;
        default:
            // Autres éléments du menu à implémenter
            break;
    }
}

// Méthodes privées

void DefaultViewManager::activateView(ViewType type) {
    if (currentView_ == type) {
        Serial.println(F("View already active"));
        return; // Déjà active
    }
    
    Serial.print(F("Activating view type: "));
    Serial.println(static_cast<int>(type));
    
    // Désactiver toutes les vues
    deactivateAllViews();
    
    // Activer la vue demandée
    currentView_ = type;
    
    switch (type) {
        case ViewType::SplashScreen:
            Serial.println(F("Activating SplashScreen"));
            splashView_->setActive(true);
            break;
        case ViewType::ParameterFocus:
            Serial.println(F("Activating ParameterFocus"));
            parameterView_->setActive(true);
            break;
        case ViewType::Menu:
            Serial.println(F("Activating Menu"));
            menuView_->setActive(true);
            break;
        default:
            // Vue par défaut
            Serial.println(F("Unknown view type - defaulting to ParameterFocus"));
            currentView_ = ViewType::ParameterFocus;
            parameterView_->setActive(true);
            break;
    }
    
    Serial.println(F("View activation complete"));
}

void DefaultViewManager::deactivateAllViews() {
    splashView_->setActive(false);
    parameterView_->setActive(false);
    menuView_->setActive(false);
    // Note: on ne désactive pas le modal ici
}