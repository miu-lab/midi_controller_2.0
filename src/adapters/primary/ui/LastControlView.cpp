#include "LastControlView.hpp"
#include "core/utils/AppStrings.hpp"

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
    return;  // Pas besoin de mise à jour continue
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
    char formatBuffer[32]; // Buffer pour la chaîne de format
    char outputBuffer[32]; // Buffer pour le résultat formaté

    // Afficher l'ID du contrôle (encodeur ou bouton)
    if (lastControlType_ == "Note On" || lastControlType_ == "Note Off") {
        // Utiliser la chaîne en mémoire Flash pour "Button: %d"
        FlashStrings::copy(formatBuffer, sizeof(formatBuffer), FMT_BUTTON_ID);
        sprintf(outputBuffer, formatBuffer, lastControlId_);
    } else {
        // Utiliser la chaîne en mémoire Flash pour "Encoder: %d"
        FlashStrings::copy(formatBuffer, sizeof(formatBuffer), FMT_ENCODER_ID);
        sprintf(outputBuffer, formatBuffer, lastControlId_);
    }
    display_->drawText(4, 15, outputBuffer);

    // Afficher le type et le canal
    FlashStrings::copy(formatBuffer, sizeof(formatBuffer), FMT_TYPE_CH);
    sprintf(outputBuffer, formatBuffer, lastControlType_.c_str(), lastChannel_);
    display_->drawText(4, 25, outputBuffer);

    // Afficher le numéro (CC ou note)
    FlashStrings::copy(formatBuffer, sizeof(formatBuffer), FMT_NUMBER);
    sprintf(outputBuffer, formatBuffer, lastNumber_);
    display_->drawText(4, 35, outputBuffer);

    // Afficher la valeur
    FlashStrings::copy(formatBuffer, sizeof(formatBuffer), FMT_VALUE);
    sprintf(outputBuffer, formatBuffer, lastValue_);
    display_->drawText(4, 45, outputBuffer);

    // Représentation graphique de la valeur sous forme de barre de progression
    int progressWidth = map(lastValue_, 0, 127, 0, 120);

    // Dessiner le contour
    display_->drawRect(4, 50, 120, 10, false);

    // Dessiner la barre de progression
    display_->drawRect(4, 50, progressWidth, 10, true);

    // Déboguer uniquement occasionnellement pour éviter de surcharger le port série
    static unsigned long lastDebugTime = 0;
    unsigned long currentTime = millis();

    if (currentTime - lastDebugTime >= 5000) {  // Débogage toutes les 5 secondes maximum
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

void LastControlView::updateLastControl(uint8_t controlId, const String& type, uint8_t channel,
                                        uint8_t number, uint8_t value) {
    // Vérifier si les données ont effectivement changé
    if (lastControlId_ != controlId || lastControlType_ != type || lastChannel_ != channel ||
        lastNumber_ != number || lastValue_ != value) {
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
        animationActive_ = false;
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