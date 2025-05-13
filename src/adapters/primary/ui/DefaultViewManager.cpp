#include "DefaultViewManager.hpp"

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

    // Initialiser chaque vue
    if (!menuView_->init() || !debugView_->init() || !controlMonitorView_->init() ||
        !contextualView_->init() || !modalView_->init()) {
        Serial.println(F("Failed to initialize one or more views"));
        return false;
    }

    // Ajouter les vues au vecteur de vues
    views_.push_back(menuView_);
    views_.push_back(debugView_);
    views_.push_back(controlMonitorView_);
    views_.push_back(contextualView_);
    views_.push_back(modalView_);

    // Par défaut, activer la vue contextuelle (écran principal)
    activeView_ = contextualView_;
    activeView_->setActive(true);

    initialized_ = true;
    return true;
}

void DefaultViewManager::update() {
    if (!initialized_) return;

    // Mettre à jour toutes les vues actives
    for (auto& view : views_) {
        if (view->isActive()) {
            view->update();
        }
    }
}

void DefaultViewManager::render() {
    if (!initialized_) return;

    // Effacer l'écran avant de rendre
    display_->clear();

    // Rendre toutes les vues actives
    // L'ordre est important - les vues de premier plan (comme les modales)
    // doivent être rendues en dernier
    for (auto& view : views_) {
        if (view->isActive()) {
            view->render();
        }
    }

    // Mettre à jour l'affichage
    display_->update();
}

void DefaultViewManager::enterMenu() {
    if (!initialized_) return;

    // Désactiver toutes les vues sauf le menu
    for (auto& view : views_) {
        view->setActive(false);
    }

    menuView_->setActive(true);
    activeView_ = menuView_;
}

void DefaultViewManager::exitMenu() {
    if (!initialized_) return;

    menuView_->setActive(false);
    contextualView_->setActive(true);
    activeView_ = contextualView_;
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

    // Désactiver toutes les vues
    for (auto& view : views_) {
        view->setActive(false);
    }

    contextualView_->setActive(true);
    activeView_ = contextualView_;
}

void DefaultViewManager::showControlMonitor() {
    if (!initialized_) return;

    // Désactiver toutes les vues
    for (auto& view : views_) {
        view->setActive(false);
    }

    controlMonitorView_->setActive(true);
    activeView_ = controlMonitorView_;
}

void DefaultViewManager::showDebugScreen() {
    if (!initialized_) return;

    // Désactiver toutes les vues
    for (auto& view : views_) {
        view->setActive(false);
    }

    debugView_->setActive(true);
    activeView_ = debugView_;
}

void DefaultViewManager::showModalDialog(const String& message) {
    if (!initialized_) return;

    modalView_->setMessage(message.c_str());
    modalView_->setActive(true);
}

void DefaultViewManager::hideModalDialog() {
    if (!initialized_) return;
    modalView_->setActive(false);
}

void DefaultViewManager::toggleModalDialogButton() {
    if (!initialized_ || !modalView_->isActive()) return;
    modalView_->toggleButton();
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
    controlMonitorView_->updateControlInfo(controlId, type, channel, number, value);
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
