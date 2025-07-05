#include "DefaultViewManager.hpp"

DefaultViewManager::DefaultViewManager(std::shared_ptr<Ili9341LvglBridge> lvglBridge,
                                     std::shared_ptr<UnifiedConfiguration> unifiedConfig,
                                     std::shared_ptr<EventBus> eventBus) 
    : lvglBridge_(lvglBridge), unifiedConfig_(unifiedConfig), eventBus_(eventBus) {
}

bool DefaultViewManager::init() {
    if (initialized_) {
        return true;
    }

    if (!lvglBridge_ || !unifiedConfig_ || !eventBus_) {
        return false;
    }

    // Créer toutes les vues LVGL
    splashView_ = std::make_shared<LvglSplashScreenView>(lvglBridge_);
    parameterView_ = std::make_shared<LvglParameterView>(lvglBridge_, unifiedConfig_, eventBus_);
    menuView_ = std::make_shared<LvglMenuView>(lvglBridge_);
    modalView_ = std::make_shared<LvglModalView>(lvglBridge_);
    
    // Initialiser
    if (!splashView_->init() || !parameterView_->init() || 
        !menuView_->init() || !modalView_->init()) {
        return false;
    }
    
    menuView_->setViewManager(this);
    
    // Activer le splash screen
    currentView_ = ViewType::SplashScreen;
    splashView_->setActive(true);
    
    initialized_ = true;
    render();
    return true;
}

void DefaultViewManager::update() {
    if (!initialized_) return;

    // Gérer la transition automatique du splash screen
    if (currentView_ == ViewType::SplashScreen) {
        if (splashView_->isSplashScreenCompleted()) {
            showHome();
            return;
        }
    }

    // Mettre à jour la vue active
    switch (currentView_) {
    case ViewType::SplashScreen:
        splashView_->update();
        break;
    case ViewType::ParameterFocus:
        parameterView_->update();
        break;
    case ViewType::Menu:
        menuView_->update();
        break;
    case ViewType::Modal:
        // Modal n'a pas besoin d'update spécifique
        break;
    }

    // Toujours mettre à jour le modal s'il est actif
    if (modalView_ && modalView_->isActive()) {
        modalView_->update();
    }

    // LVGL views need constant rendering
    render();
}

void DefaultViewManager::render() {
    if (!initialized_) return;

    // Rendre la vue active
    switch (currentView_) {
    case ViewType::SplashScreen:
        splashView_->render();
        break;
    case ViewType::ParameterFocus:
        parameterView_->render();
        break;
    case ViewType::Menu:
        menuView_->render();
        break;
    case ViewType::Modal:
        // Modal est rendu séparément après
        break;
    }

    // Rendre le modal par-dessus si actif
    if (modalView_ && modalView_->isActive()) {
        modalView_->render();
    }
}

void DefaultViewManager::showParameterFocus(uint8_t ccNumber, uint8_t channel, uint8_t value, const String& parameterName) {
    if (!initialized_) return;
    activateView(ViewType::ParameterFocus);
}

void DefaultViewManager::updateParameterValue(uint8_t value) {
    if (!initialized_ || currentView_ != ViewType::ParameterFocus) return;
    // TODO: Ajouter méthode updateValue à LvglParameterView
}

void DefaultViewManager::showMenu() {
    if (!initialized_) return;
    activateView(ViewType::Menu);
}

void DefaultViewManager::showHome() {
    if (!initialized_) return;
    activateView(ViewType::ParameterFocus);
}

void DefaultViewManager::showModal(const String& message) {
    if (!initialized_) return;
    modalView_->setMessage(message.c_str());
    modalView_->setActive(true);
    render();
}

void DefaultViewManager::hideModal() {
    if (!initialized_) return;
    modalView_->setActive(false);
    render();
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
    
    int selectedIndex = menuView_->getCurrentIndex();
    switch (selectedIndex) {
    case 0:
        showHome();
        break;
    default:
        break;
    }
}

void DefaultViewManager::activateView(ViewType type) {
    if (currentView_ == type) return;

    deactivateAllViews();
    currentView_ = type;

    switch (type) {
    case ViewType::SplashScreen:
        splashView_->setActive(true);
        break;
    case ViewType::ParameterFocus:
        parameterView_->setActive(true);
        break;
    case ViewType::Menu:
        menuView_->setActive(true);
        break;
    default:
        currentView_ = ViewType::ParameterFocus;
        parameterView_->setActive(true);
        break;
    }
}

void DefaultViewManager::deactivateAllViews() {
    splashView_->setActive(false);
    parameterView_->setActive(false);
    menuView_->setActive(false);
}