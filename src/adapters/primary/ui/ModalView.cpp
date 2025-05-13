#include "ModalView.hpp"

ModalView::ModalView(std::shared_ptr<DisplayPort> display)
    : View(display),
      okSelected_(true),
      hasCancelButton_(false) {}

bool ModalView::init() {
    title_ = "Message";
    message_ = "";
    okLabel_ = "OK";
    cancelLabel_ = "Cancel";
    
    return true;
}

void ModalView::update() {
    // Rien à mettre à jour périodiquement
}

void ModalView::render() {
    if (!active_) return;
    
    // Dessiner un fond semi-opaque pour masquer partiellement le contenu en arrière-plan
    // Fond transparent : remplir un rectangle avec une alternance de pixels
    for (int y = 6; y < 58; y += 2) {
        for (int x = 8; x < 120; x += 2) {
            display_->drawRect(x, y, 1, 1, true);
        }
    }
    
    // Dessiner la boîte de dialogue - fond noir avec bordure blanche
    display_->drawRect(10, 8, 108, 48, false); // Bordure blanche
    
    // Créer une barre de titre (rectangle blanc en haut)
    display_->drawRect(11, 9, 106, 10, true);
    
    // Dessiner le titre en noir sur la barre blanche
    display_->setTextColor(0); // Texte noir 
    display_->drawText(14, 10, title_.c_str());
    display_->setTextColor(1); // Revenir au texte blanc pour le reste
    
    // Découper le message en lignes
    std::vector<String> lines = wrapText(message_);
    
    // Dessiner une ligne horizontale sous la barre de titre
    display_->drawLine(11, 21, 116, 21);
    
    // Afficher le message
    int y = 24;
    for (const auto& line : lines) {
        display_->drawText(14, y, line.c_str());
        y += 9; // Espacement des lignes
        
        // Limiter le nombre de lignes affichées
        if (y > 48) break;
    }
    
    // Dessiner les boutons
    int buttonY = 48;
    
    if (hasCancelButton_) {
        // Deux boutons
        int okX = 30;
        int cancelX = 75;
        
        // Bouton OK
        display_->drawRect(okX - 15, buttonY - 2, 30, 12, okSelected_);
        display_->drawText(okX - okLabel_.length() * 3 + 2, buttonY, okLabel_.c_str());
        
        // Bouton Cancel
        display_->drawRect(cancelX - 15, buttonY - 2, 30, 12, !okSelected_);
        display_->drawText(cancelX - cancelLabel_.length() * 3 + 2, buttonY, cancelLabel_.c_str());
    } else {
        // Un seul bouton (OK)
        int okX = 64;
        display_->drawRect(okX - 15, buttonY - 2, 30, 12, true);
        display_->drawText(okX - okLabel_.length() * 3 + 2, buttonY, okLabel_.c_str());
    }
}

bool ModalView::handleEvent(uint8_t eventType, int32_t data) {
    // Traitement des événements d'entrée
    // Cette implémentation simplifiée ne gère pas les événements directement
    return false;
}

bool ModalView::isActive() const {
    return active_;
}

void ModalView::setActive(bool active) {
    active_ = active;
    
    // Réinitialiser la sélection de bouton quand la vue devient active
    if (active) {
        okSelected_ = true;
    }
}

void ModalView::setMessage(const char* message) {
    message_ = message;
}

void ModalView::setTitle(const char* title) {
    title_ = title;
}

void ModalView::setButtonLabels(const char* okLabel, const char* cancelLabel) {
    okLabel_ = okLabel;
    
    if (cancelLabel && strlen(cancelLabel) > 0) {
        cancelLabel_ = cancelLabel;
        hasCancelButton_ = true;
    } else {
        cancelLabel_ = "";
        hasCancelButton_ = false;
    }
}

void ModalView::toggleButton() {
    if (hasCancelButton_) {
        okSelected_ = !okSelected_;
    }
}

bool ModalView::isOkSelected() const {
    return okSelected_;
}

std::vector<String> ModalView::wrapText(const String& message) {
    std::vector<String> lines;
    
    // Largeur maximale en caractères (pour une police de taille standard)
    const int MAX_CHARS_PER_LINE = 18;
    
    int startPos = 0;
    int length = message.length();
    
    while (startPos < length) {
        int endPos = startPos + MAX_CHARS_PER_LINE;
        
        if (endPos >= length) {
            // Dernière ligne
            lines.push_back(message.substring(startPos));
            break;
        }
        
        // Chercher un espace pour couper proprement
        int breakPos = endPos;
        while (breakPos > startPos && message.charAt(breakPos) != ' ') {
            breakPos--;
        }
        
        if (breakPos == startPos) {
            // Pas d'espace trouvé, couper au maximum
            breakPos = endPos;
        }
        
        // Ajouter la ligne
        lines.push_back(message.substring(startPos, breakPos));
        
        // Passer à la position suivante (après l'espace)
        startPos = breakPos + 1;
    }
    
    return lines;
}
