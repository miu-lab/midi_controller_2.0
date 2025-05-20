#include "LastControlView.hpp"

#include <math.h>

#include "adapters/secondary/hardware/display/Ssd1306Display.hpp"
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

    char outputBuffer[32];  // Buffer pour le résultat formaté

    // --- DESIGN MINIMALISTE ---

    // 1. Identifiant du contrôle (en haut à gauche, format court)
    if (lastControlType_ == "ON" || lastControlType_ == "OFF") {
        sprintf(outputBuffer, "BTN %d", lastControlId_);
    } else {
        sprintf(outputBuffer, "ENC %d", lastControlId_);
    }
    display_->drawText(4, 12, outputBuffer);

    // 2. Type et canal MIDI (en haut à droite, format minimal)
    sprintf(outputBuffer,
            "%s %d",
            lastControlType_ == "Control Change" ? "CC"
            : lastControlType_ == "Note On"      ? "NOTE"
                                                 : lastControlType_.c_str(),
            lastNumber_);
    display_->drawText(124 - strlen(outputBuffer) * 6, 12, outputBuffer);

    // 3. Cercle extérieur (contour) - légèrement plus petit
    display_->drawCircle(64, 35, 22, false);

    // 4. Visualisation circulaire de la valeur sur 270 degrés
    int endAngle = map(lastValue_, 0, 127, -225, 45);  // 270 degrés de -135 à +135

    // Vérifier si nous avons accès à un display SSD1306 pour utiliser drawArc
    auto* ssd1306Display = static_cast<Ssd1306Display*>(display_.get());
    ssd1306Display
        ->drawArc(64, 35, 22, -225, endAngle, 1, 5);  // 1 = couleur blanche, 2 = épaisseur

    // 5. Valeur MIDI au centre (grand format)
    sprintf(outputBuffer, "%d", lastValue_);
    // Centrage approximatif basé sur le nombre de chiffres
    int offsetX = lastValue_ < 10 ? 3 : (lastValue_ < 100 ? 6 : 9);
    display_->drawText(64 - offsetX, 30, outputBuffer);  // Remonte de 1 pixel

    // // Centrage approximatif
    // display_->drawText(64 - strlen(outputBuffer) * 3, 48, outputBuffer);  // Remonte de 4 pixels
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