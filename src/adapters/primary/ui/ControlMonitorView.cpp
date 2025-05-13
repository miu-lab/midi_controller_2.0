#include "ControlMonitorView.hpp"

ControlMonitorView::ControlMonitorView(std::shared_ptr<DisplayPort> display)
    : View(display),
      selectedControlIndex_(0),
      scrollOffset_(0),
      maxVisibleControls_(3),
      lastUpdateTime_(0) {}

bool ControlMonitorView::init() {
    // Initialiser avec quelques valeurs par défaut pour les tests
    updateControlInfo(0, "CC", 1, 7, 64);  // Volume
    updateControlInfo(1, "Note On", 1, 60, 127);  // C3 Note On
    updateControlInfo(2, "CC", 1, 10, 64);  // Pan
    
    // Initialiser les positions des encodeurs
    updateEncoderPosition(0, 64);
    updateEncoderPosition(1, 127);
    updateEncoderPosition(2, 64);
    
    // Initialiser les états des boutons
    updateButtonState(0, false);
    updateButtonState(1, false);
    updateButtonState(2, false);
    
    return true;
}

void ControlMonitorView::update() {
    // Rien à mettre à jour périodiquement dans cette vue
    // Les mises à jour sont déclenchées par des événements externes
}

void ControlMonitorView::render() {
    if (!active_) return;
    
    // Dessiner le cadre
    display_->drawRect(0, 0, 128, 64, false);
    
    // Dessiner le titre
    display_->drawText(4, 1, "CONTROL MONITOR");
    display_->drawLine(0, 10, 128, 10);
    
    // Afficher les informations de contrôle visibles
    int startY = 13;
    int controlHeight = 16;
    
    for (int i = 0; i < maxVisibleControls_ && (i + scrollOffset_) < static_cast<int>(controlInfos_.size()); i++) {
        int infoIndex = i + scrollOffset_;
        int y = startY + (i * controlHeight);
        
        const ControlInfo& info = controlInfos_[infoIndex];
        
        // Mettre en évidence le contrôle sélectionné
        if (infoIndex == selectedControlIndex_) {
            display_->drawRect(2, y - 1, 124, controlHeight - 2, false);
        }
        
        // Formater l'affichage des informations
        char buffer[32];
        sprintf(buffer, "ID:%d %s Ch:%d", info.controlId, info.type.c_str(), info.channel);
        display_->drawText(4, y, buffer);
        
        sprintf(buffer, "Num:%d Val:%d", info.number, info.value);
        display_->drawText(4, y + 8, buffer);
    }
    
    // Indicateurs de défilement si nécessaire
    if (scrollOffset_ > 0) {
        // Flèche vers le haut
        display_->drawLine(124, 13, 127, 16);
        display_->drawLine(124, 13, 121, 16);
    }
    
    if ((scrollOffset_ + maxVisibleControls_) < static_cast<int>(controlInfos_.size())) {
        // Flèche vers le bas
        display_->drawLine(124, 61, 127, 58);
        display_->drawLine(124, 61, 121, 58);
    }
}

bool ControlMonitorView::handleEvent(uint8_t eventType, int32_t data) {
    // Traitement des événements d'entrée
    // Cette implémentation simplifiée ne gère pas les événements directement
    return false;
}

bool ControlMonitorView::isActive() const {
    return active_;
}

void ControlMonitorView::setActive(bool active) {
    active_ = active;
}

void ControlMonitorView::updateControlInfo(uint8_t controlId, const String& type,
                                        uint8_t channel, uint8_t number, uint8_t value) {
    // Rechercher un contrôle existant avec le même ID
    bool found = false;
    for (auto& info : controlInfos_) {
        if (info.controlId == controlId) {
            // Mettre à jour le contrôle existant
            info.type = type;
            info.channel = channel;
            info.number = number;
            info.value = value;
            info.timestamp = millis();
            found = true;
            break;
        }
    }
    
    // Si le contrôle n'a pas été trouvé, en ajouter un nouveau
    if (!found) {
        controlInfos_.emplace_back(controlId, type, channel, number, value);
        
        // Limiter le nombre d'entrées pour éviter les fuites de mémoire
        const size_t MAX_CONTROL_ENTRIES = 50;
        while (controlInfos_.size() > MAX_CONTROL_ENTRIES) {
            controlInfos_.erase(controlInfos_.begin());
            
            // Ajuster l'index sélectionné et le décalage de défilement si nécessaire
            if (selectedControlIndex_ > 0) {
                selectedControlIndex_--;
            }
            if (scrollOffset_ > 0) {
                scrollOffset_--;
            }
        }
    }
}

void ControlMonitorView::updateEncoderPosition(uint8_t encoderId, int32_t position) {
    // Rechercher un encodeur existant avec le même ID
    bool found = false;
    for (auto& pair : encoderPositions_) {
        if (pair.first == encoderId) {
            // Mettre à jour la position de l'encodeur existant
            pair.second = position;
            found = true;
            break;
        }
    }
    
    // Si l'encodeur n'a pas été trouvé, l'ajouter
    if (!found) {
        encoderPositions_.emplace_back(std::make_pair(encoderId, position));
    }
    
    // Logique supplémentaire pour mettre à jour les contrôles associés
    // pourrait être ajoutée ici
}

void ControlMonitorView::updateEncoderButtonState(uint8_t encoderId, bool pressed) {
    // Mettre à jour l'état du bouton d'encodeur est similaire à updateButtonState
    updateButtonState(encoderId, pressed);
    
    // Logique spécifique aux boutons d'encodeur pourrait être ajoutée ici
}

void ControlMonitorView::updateButtonState(uint8_t buttonId, bool pressed) {
    // Rechercher un bouton existant avec le même ID
    bool found = false;
    for (auto& pair : buttonStates_) {
        if (pair.first == buttonId) {
            // Mettre à jour l'état du bouton existant
            pair.second = pressed;
            found = true;
            break;
        }
    }
    
    // Si le bouton n'a pas été trouvé, l'ajouter
    if (!found) {
        buttonStates_.emplace_back(std::make_pair(buttonId, pressed));
    }
    
    // Logique supplémentaire pour mettre à jour les contrôles associés
    // pourrait être ajoutée ici
}

void ControlMonitorView::scroll(int8_t delta) {
    // Calculer le nouveau décalage de défilement
    int newOffset = scrollOffset_ - delta;
    
    // Limiter le défilement aux limites des entrées de contrôle
    newOffset = max(0, min(newOffset, static_cast<int>(controlInfos_.size()) - maxVisibleControls_));
    
    // Mettre à jour le décalage
    scrollOffset_ = newOffset;
}
