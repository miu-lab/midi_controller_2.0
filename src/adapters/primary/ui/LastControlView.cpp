#include "LastControlView.hpp"

LastControlView::LastControlView(std::shared_ptr<DisplayPort> display)
    : View(display), 
      lastControlId_(0), 
      lastChannel_(1), 
      lastNumber_(0), 
      lastValue_(0),
      lastUpdateTime_(0),
      animationActive_(false),
      animationStartTime_(0) {
    lastControlType_ = "CC";
}

bool LastControlView::init() {
    title_ = "Last Control";
    return true;
}

void LastControlView::update() {
    // Animation pour la mise à jour des valeurs
    if (animationActive_) {
        unsigned long currentTime = millis();
        if (currentTime - animationStartTime_ > ANIMATION_DURATION) {
            animationActive_ = false;
        }
    }
    
    // Une fois par seconde, vérifier que les données de contrôle sont affichées
    static unsigned long lastCheckTime = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastCheckTime >= 1000) {
        lastCheckTime = currentTime;
        Serial.print(F("LastControlView status - active: "));
        Serial.print(active_ ? F("YES") : F("NO"));
        Serial.print(F(", ID: "));
        Serial.print(lastControlId_);
        Serial.print(F(", Type: "));
        Serial.print(lastControlType_);
        Serial.print(F(", CH: "));
        Serial.print(lastChannel_);
        Serial.print(F(", Num: "));
        Serial.print(lastNumber_);
        Serial.print(F(", Val: "));
        Serial.println(lastValue_);
    }
}

void LastControlView::render() {
    if (!active_) {
        return;
    }
    
    // Effacer l'écran (important pour éviter les superpositions)
    // Note: Pas besoin de clear ici, car DefaultViewManager::render() le fait déjà
    // display_->clear();
    
    // Dessiner le cadre
    display_->drawRect(0, 0, 128, 64, false);
    
    // Dessiner le titre
    display_->drawText(4, 2, title_.c_str());
    display_->drawLine(0, 10, 128, 10);
    
    // Informations sur le contrôle
    char buffer[32];
    
    // Afficher l'ID du contrôle (encodeur ou bouton)
    if (lastControlType_ == "Note On" || lastControlType_ == "Note Off") {
        sprintf(buffer, "Button: %d", lastControlId_);
    } else {
        sprintf(buffer, "Encoder: %d", lastControlId_);
    }
    display_->drawText(4, 15, buffer);
    
    // Afficher le type et le canal
    sprintf(buffer, "Type: %s  Ch: %d", lastControlType_.c_str(), lastChannel_);
    display_->drawText(4, 25, buffer);
    
    // Afficher le numéro (CC ou note)
    sprintf(buffer, "Number: %d", lastNumber_);
    display_->drawText(4, 35, buffer);
    
    // Afficher la valeur
    sprintf(buffer, "Value: %d", lastValue_);
    display_->drawText(4, 45, buffer);
    
    // Représentation graphique de la valeur sous forme de barre de progression
    int progressWidth = map(lastValue_, 0, 127, 0, 120);
    
    // Dessiner le contour
    display_->drawRect(4, 50, 120, 10, false);
    
    // Remplir la barre avec pulsation si animation active
    if (animationActive_) {
        unsigned long elapsedTime = millis() - animationStartTime_;
        int pulseEffect = sin(elapsedTime * 0.03) * 3; // Effet de pulsation
        
        // Assurer que la largeur ne dépasse pas les limites
        progressWidth = max(0, min(120, progressWidth + pulseEffect));
    }
    
    // Dessiner la barre de progression
    display_->drawRect(4, 50, progressWidth, 10, true);
    
    // Déboguer uniquement occasionnellement pour éviter de surcharger le port série
    static unsigned long lastDebugTime = 0;
    unsigned long currentTime = millis();
    
    if (currentTime - lastDebugTime >= 5000) { // Débogage toutes les 5 secondes maximum
        lastDebugTime = currentTime;
        Serial.print(F("LastControlView rendering - ID="));
        Serial.print(lastControlId_);
        Serial.print(F(" Val="));
        Serial.println(lastValue_);
    }
}

bool LastControlView::handleEvent(uint8_t eventType, int32_t data) {
    // Cette vue ne traite pas directement les événements
    return false;
}

bool LastControlView::isActive() const {
    return active_;
}

void LastControlView::setActive(bool active) {
    if (active_ != active) {
        Serial.print(F("LastControlView: Setting active state to "));
        Serial.println(active ? F("true") : F("false"));
        active_ = active;
        
        // Marquer comme sale lors du changement d'état d'activation
        setDirty(true);
    }
}

void LastControlView::updateLastControl(uint8_t controlId, const String& type, 
                                     uint8_t channel, uint8_t number, uint8_t value) {
    // Vérifier si les données ont effectivement changé
    if (lastControlId_ != controlId || lastControlType_ != type || 
        lastChannel_ != channel || lastNumber_ != number || lastValue_ != value) {
        
        Serial.println(F("LastControlView: Control information updated"));

        // Mise à jour des valeurs
        lastControlId_ = controlId;
        lastControlType_ = type;
        lastChannel_ = channel;
        lastNumber_ = number;
        lastValue_ = value;
        
        // Mettre à jour l'horodatage
        lastUpdateTime_ = millis();
        
        // Activer l'animation
        animationActive_ = true;
        animationStartTime_ = millis();
        
        // Marquer la vue comme sale pour forcer le rendu lors de la prochaine mise à jour
        setDirty(true);
        
        // Assurer que la vue est active si elle n'est pas déjà
        if (!active_) {
            Serial.println(F("LastControlView: Activating view after control update"));
            active_ = true;
        }
    }
}