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

    // Vérifier si nous avons accès à un display SSD1306 pour utiliser les fonctionnalités avancées
    auto* ssd1306Display = static_cast<Ssd1306Display*>(display_.get());

    // 1. Cercle extérieur (contour)
    ssd1306Display->drawCircle(64, 32, 12, false, SSD1306_WHITE);

    // 2. Visualisation circulaire de la valeur sur 270 degrés
    // Le nouvel algorithme déterministe élimine automatiquement le clignottement
    int endAngle = map(lastValue_, 0, 127, -225, 45);  // 270 degrés de -225 à +45
    ssd1306Display->drawArc(64, 32, 15, -225, endAngle, SSD1306_WHITE, 4);  // Arc avec épaisseur 4

    // 3. Conversion en texte
    char valueBuffer[8];
    sprintf(valueBuffer, "%d", lastValue_);

    // 4. Afficher la valeur au centre
    ssd1306Display->setTextSize(1);
    ssd1306Display->drawCenteredText(64, 28, valueBuffer);

    // 5. Numéro de CC/note en dessous, format discret
    char ccBuffer[16];
    if (lastControlType_ == "Control Change" || lastControlType_ == "CC") {
        sprintf(ccBuffer, "CC %d", lastNumber_);
    } else {
        sprintf(ccBuffer, "NOTE %d", lastNumber_);
    }
    ssd1306Display->drawCenteredText(64, 48, ccBuffer);
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

        // Assurer que la vue est active si elle n'est pas déjà
        if (!active_) {
            Serial.println(F("LastControlView: Activating view after control update"));
            active_ = true;
        }
    }
}