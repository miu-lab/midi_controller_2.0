#include "SplashScreenView.hpp"

SplashScreenView::SplashScreenView(std::shared_ptr<DisplayPort> display)
    : View(display), startTime_(0), duration_(2000) {} // 2000 ms = 2 secondes

bool SplashScreenView::init() {
    title_ = "MIDI Controller";
    subtitle_ = "Welcome";
    version_ = "v1.0";
    
    return true;
}

void SplashScreenView::update() {
    // Si la vue vient d'être activée, initialiser le temps de départ
    if (active_ && startTime_ == 0) {
        startTime_ = millis();
    }
}

void SplashScreenView::render() {
    if (!active_) return;
    
    // Dessiner le cadre
    display_->drawRect(0, 0, 128, 64, false);
    
    // Titre centré en haut
    int titleWidth = title_.length() * 6; // Approximatif pour la police standard
    int titleX = (128 - titleWidth) / 2;
    display_->drawText(titleX, 15, title_.c_str());
    
    // Sous-titre centré au milieu
    int subtitleWidth = subtitle_.length() * 6;
    int subtitleX = (128 - subtitleWidth) / 2;
    display_->drawText(subtitleX, 30, subtitle_.c_str());
    
    // Version en bas à droite
    display_->drawText(85, 50, version_.c_str());

    // Animation : afficher une barre de progression qui représente le temps écoulé
    unsigned long currentTime = millis();
    unsigned long elapsed = currentTime - startTime_;
    
    if (elapsed <= duration_) {
        int progressWidth = map(elapsed, 0, duration_, 0, 108);
        display_->drawRect(10, 45, progressWidth, 4, true); // Barre de progression remplie
        display_->drawRect(10, 45, 108, 4, false); // Contour de la barre
    }
}

bool SplashScreenView::handleEvent(uint8_t eventType, int32_t data) {
    // Le splash screen ignore tous les événements
    return false;
}

bool SplashScreenView::isActive() const {
    return active_;
}

void SplashScreenView::setActive(bool active) {
    active_ = active;
    
    // Réinitialiser le temps de départ quand la vue devient active
    if (active) {
        startTime_ = millis();
    } else {
        startTime_ = 0;
    }
}

bool SplashScreenView::isSplashScreenCompleted() const {
    if (!active_) return true;
    
    unsigned long currentTime = millis();
    return (currentTime - startTime_ >= duration_);
}
