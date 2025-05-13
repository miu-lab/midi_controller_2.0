#include "ContextualView.hpp"

ContextualView::ContextualView(std::shared_ptr<DisplayPort> display)
    : View(display), scrollOffset_(0), lastUpdateTime_(0) {
    // Initialisation des tableaux
    for (int i = 0; i < MAX_BUTTONS; i++) {
        buttonStates_[i] = false;
    }
}

bool ContextualView::init() {
    title_ = "MIDI Controller";
    message_ = "Ready";
    activeProfile_ = "Default";
    
    // Initialiser les états par défaut
    for (int i = 0; i < MAX_ENCODERS; i++) {
        encoders_[i].position = 0;
        encoders_[i].buttonPressed = false;
    }
    
    return true;
}

void ContextualView::update() {
    // Mise à jour périodique si nécessaire
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime_ > 1000) { // Toutes les secondes
        lastUpdateTime_ = currentTime;
        
        // Mettre à jour les informations dynamiques si nécessaire
    }
}

void ContextualView::render() {
    if (!active_) return;
    
    // Dessiner le cadre
    display_->drawRect(0, 0, 128, 64, false);
    
    // Dessiner le titre
    display_->drawText(4, 1, title_.c_str());
    display_->drawLine(0, 10, 128, 10);
    
    // Afficher le profil actif
    display_->drawText(4, 13, ("Profile: " + activeProfile_).c_str());
    
    // Afficher le message principal
    display_->drawText(4, 24, message_.c_str());
    
    // Afficher les états des encodeurs (version simplifiée)
    int startY = 35;
    for (int i = 0; i < 4 && i < MAX_ENCODERS; i++) {
        char buffer[16];
        sprintf(buffer, "E%d:%3ld %s", i, encoders_[i].position, 
                encoders_[i].buttonPressed ? "P" : " ");
        display_->drawText(4 + (i % 2) * 64, startY + (i / 2) * 10, buffer);
    }
    
    // Afficher les états des boutons (version simplifiée)
    display_->drawText(4, 55, "Buttons:");
    for (int i = 0; i < 8 && i < MAX_BUTTONS; i++) {
        // Afficher un carré plein pour les boutons pressés, vide pour les autres
        int x = 50 + i * 9;
        display_->drawRect(x, 55, 8, 8, buttonStates_[i]);
    }
}

bool ContextualView::handleEvent(uint8_t eventType, int32_t data) {
    // Traitement des événements d'entrée
    // Cette implémentation simplifiée ne gère pas les événements directement
    return false;
}

bool ContextualView::isActive() const {
    return active_;
}

void ContextualView::setActive(bool active) {
    active_ = active;
}

void ContextualView::setTitle(const String& title) {
    title_ = title;
}

void ContextualView::setMessage(const String& message) {
    message_ = message;
}

void ContextualView::scroll(int delta) {
    // Pour l'instant, cette vue ne gère pas le défilement
    // Pourrait être implémenté si la vue devient plus complexe
}

void ContextualView::updateEncoderPosition(uint8_t encoderId, int32_t position) {
    if (encoderId < MAX_ENCODERS) {
        encoders_[encoderId].position = position;
    }
}

void ContextualView::updateEncoderButtonState(uint8_t encoderId, bool pressed) {
    if (encoderId < MAX_ENCODERS) {
        encoders_[encoderId].buttonPressed = pressed;
    }
}

void ContextualView::updateButtonState(uint8_t buttonId, bool pressed) {
    if (buttonId < MAX_BUTTONS) {
        buttonStates_[buttonId] = pressed;
    }
}

void ContextualView::setActiveProfile(const String& profileName) {
    activeProfile_ = profileName;
}
