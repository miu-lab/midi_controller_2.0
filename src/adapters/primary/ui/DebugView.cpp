#include "DebugView.hpp"

DebugView::DebugView(std::shared_ptr<DisplayPort> display)
    : View(display),
      scrollOffset_(0),
      maxVisibleLines_(5),
      lastUpdateTime_(0),
      autoScroll_(true) {}

bool DebugView::init() {
    // Ajouter quelques entrées de log pour les tests
    addLogEntry("Debug view initialized");
    addLogEntry("MIDI system ready");
    addLogEntry("Input system ready");
    addLogEntry("UI system ready");
    return true;
}

void DebugView::update() {
    // Ajouter périodiquement des logs de test pendant le développement
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime_ > 5000) { // Toutes les 5 secondes
        lastUpdateTime_ = currentTime;
        
        // En production, cette fonction ne ferait rien ici, car les logs
        // seraient ajoutés par d'autres parties du système
    }
    
    // Si l'auto-scroll est activé, toujours afficher les dernières entrées
    if (autoScroll_ && !logEntries_.empty()) {
        scrollOffset_ = max(0, static_cast<int>(logEntries_.size()) - maxVisibleLines_);
    }
}

void DebugView::render() {
    if (!active_) return;
    
    // Dessiner le cadre
    display_->drawRect(0, 0, 128, 64, false);
    
    // Dessiner le titre
    display_->drawText(4, 1, "DEBUG LOG");
    display_->drawLine(0, 10, 128, 10);
    
    // Afficher les entrées de log visibles
    int startY = 13;
    int lineHeight = 10;
    
    for (int i = 0; i < maxVisibleLines_ && (i + scrollOffset_) < static_cast<int>(logEntries_.size()); i++) {
        int entryIndex = i + scrollOffset_;
        int y = startY + (i * lineHeight);
        
        // Afficher le texte de l'entrée (tronqué si nécessaire)
        String displayText = logEntries_[entryIndex];
        if (displayText.length() > 21) { // Limiter la longueur du texte
            displayText = displayText.substring(0, 18) + "...";
        }
        
        display_->drawText(4, y, displayText.c_str());
    }
    
    // Indicateurs de défilement si nécessaire
    if (scrollOffset_ > 0) {
        // Flèche vers le haut
        display_->drawLine(124, 13, 127, 16);
        display_->drawLine(124, 13, 121, 16);
    }
    
    if ((scrollOffset_ + maxVisibleLines_) < static_cast<int>(logEntries_.size())) {
        // Flèche vers le bas
        display_->drawLine(124, 61, 127, 58);
        display_->drawLine(124, 61, 121, 58);
    }
    
    // Indicateur d'auto-scroll
    if (autoScroll_) {
        display_->drawText(4, 54, "AUTO");
    }
}

bool DebugView::handleEvent(uint8_t eventType, int32_t data) {
    // Traitement des événements d'entrée
    // Cette implémentation simplifiée ne gère pas les événements directement
    return false;
}

bool DebugView::isActive() const {
    return active_;
}

void DebugView::setActive(bool active) {
    active_ = active;
    
    // Réinitialiser l'auto-scroll lorsque la vue devient active
    if (active) {
        autoScroll_ = true;
        update(); // Forcer une mise à jour pour positionner le scroll
    }
}

void DebugView::addLogEntry(const String& message) {
    logEntries_.push_back(message);
    
    // Limiter le nombre d'entrées pour éviter les fuites de mémoire
    const size_t MAX_LOG_ENTRIES = 100;
    while (logEntries_.size() > MAX_LOG_ENTRIES) {
        logEntries_.erase(logEntries_.begin());
    }
    
    // Mettre à jour le défilement si l'auto-scroll est activé
    if (autoScroll_) {
        scrollOffset_ = max(0, static_cast<int>(logEntries_.size()) - maxVisibleLines_);
    }
}

void DebugView::scroll(int8_t delta) {
    // Désactiver l'auto-scroll lorsque l'utilisateur fait défiler manuellement
    if (delta != 0) {
        autoScroll_ = false;
    }
    
    // Calculer le nouveau décalage de défilement
    int newOffset = scrollOffset_ - delta;
    
    // Limiter le défilement aux limites des entrées de log
    newOffset = max(0, min(newOffset, static_cast<int>(logEntries_.size()) - maxVisibleLines_));
    
    // Mettre à jour le décalage
    scrollOffset_ = newOffset;
}

void DebugView::clearLogs() {
    logEntries_.clear();
    scrollOffset_ = 0;
}
