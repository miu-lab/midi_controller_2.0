#include "ParameterFocusView.hpp"

#include <math.h>

#include "adapters/secondary/hardware/display/Ssd1306Display.hpp"
#include "core/utils/AppStrings.hpp"

ParameterFocusView::ParameterFocusView(std::shared_ptr<DisplayPort> display)
    : View(display),
      ccNumber_(0),
      channel_(1),
      value_(0),
      parameterName_("PARAM"),
      needsRedraw_(false),
      lastUpdateTime_(0) {}

bool ParameterFocusView::init() {
    // Paramètres par défaut
    ccNumber_ = 0;
    channel_ = 1;
    value_ = 0;
    parameterName_ = "PARAM";
    needsRedraw_ = true;

    return true;
}

void ParameterFocusView::update() {
    // Pas de mise à jour continue nécessaire pour cette vue
    return;
}

void ParameterFocusView::render() {
    if (!active_ || !needsRedraw_) {
        Serial.println(F("ParameterFocusView::render() - not active or no redraw needed"));
        return;
    }

    Serial.println(F("ParameterFocusView::render() - Starting render"));

    // NE PAS FAIRE display_->clear() ici - c'est déjà fait par le ViewManager !

    drawLabels();
    drawParameterCircle();
    drawParameterName();

    needsRedraw_ = false;

    Serial.println(F("ParameterFocusView::render() - Render complete"));
}

bool ParameterFocusView::handleEvent(uint8_t eventType, int32_t data) {
    // Cette vue ne traite pas directement les événements
    // Elle est mise à jour via les méthodes setParameter/setValue
    return false;
}

bool ParameterFocusView::isActive() const {
    return active_;
}

void ParameterFocusView::setActive(bool active) {
    if (active_ != active) {
        active_ = active;
        if (active) {
            Serial.println(F("ParameterFocusView: Activated - forcing redraw"));
            needsRedraw_ = true;  // FORCER le redraw quand on active
        }
        setDirty(true);
    }
}

void ParameterFocusView::setParameter(uint8_t ccNumber, uint8_t channel, uint8_t value,
                                      const String& parameterName) {
    bool hasChanged = (ccNumber_ != ccNumber || channel_ != channel || value_ != value ||
                       parameterName_ != parameterName);

    if (hasChanged) {
        ccNumber_ = ccNumber;
        channel_ = channel;
        value_ = value;
        parameterName_ = parameterName;

        lastUpdateTime_ = millis();
        needsRedraw_ = true;
        setDirty(true);

        // Activer la vue automatiquement si elle ne l'est pas
        if (!active_) {
            active_ = true;
        }
    }
}

void ParameterFocusView::setValue(uint8_t value) {
    if (value_ != value) {
        value_ = value;
        lastUpdateTime_ = millis();
        needsRedraw_ = true;
        setDirty(true);

        // Activer la vue automatiquement si elle ne l'est pas
        if (!active_) {
            active_ = true;
        }
    }
}

void ParameterFocusView::setParameterName(const String& parameterName) {
    if (parameterName_ != parameterName) {
        parameterName_ = parameterName;
        needsRedraw_ = true;
        setDirty(true);
    }
}

void ParameterFocusView::drawLabels() {
    Serial.println(F("ParameterFocusView::drawLabels() - Drawing labels"));

    // CC numéro en haut à gauche
    display_->setTextSize(1);
    display_->drawFormattedText(CC_LABEL_X, CC_LABEL_Y, "CC %d", ccNumber_);

    // Canal MIDI en haut à droite
    display_->drawFormattedText(CH_LABEL_X, CH_LABEL_Y, "CH %d", channel_);

    Serial.println(F("ParameterFocusView::drawLabels() - Labels drawn"));
}

void ParameterFocusView::drawParameterCircle() {
    auto* ssd1306Display = static_cast<Ssd1306Display*>(display_.get());

    // Dessiner le cercle de base (contour)
    ssd1306Display->drawCircle(CIRCLE_CENTER_X,
                               CIRCLE_CENTER_Y,
                               CIRCLE_RADIUS,
                               false,
                               SSD1306_WHITE);

    // Calculer et dessiner le remplissage
    float fillRatio = calculateFillRatio();
    drawFilledCircleArc(CIRCLE_CENTER_X,
                        CIRCLE_CENTER_Y,
                        CIRCLE_RADIUS - CIRCLE_THICKNESS,
                        fillRatio);
}

void ParameterFocusView::drawParameterName() {
    // Nom du paramètre centré en bas
    display_->setTextSize(1);
    display_->drawCenteredText(CIRCLE_CENTER_X, PARAM_NAME_Y, parameterName_.c_str());
}

void ParameterFocusView::drawFilledCircleArc(int centerX, int centerY, int radius,
                                             float fillRatio) {
    auto* ssd1306Display = static_cast<Ssd1306Display*>(display_.get());

    if (fillRatio <= 0.0f) {
        return;  // Rien à dessiner
    }

    // Limiter le ratio entre 0 et 1
    fillRatio = constrain(fillRatio, 0.0f, 1.0f);

    // Calculer l'angle final basé sur le ratio (cercle complet = 360°)
    // Commencer à -90° (haut du cercle) et aller dans le sens horaire
    int startAngle = -225;
    int sweepAngle = (int)(fillRatio * 270.0f);
    int endAngle = startAngle + sweepAngle;

    // Utiliser la fonction drawArc existante avec plusieurs passes pour créer un effet de
    // remplissage
    for (int r = radius - CIRCLE_THICKNESS; r < radius; r += CIRCLE_THICKNESS) {
        ssd1306Display
            ->drawArc(centerX, centerY, r, startAngle, endAngle, SSD1306_WHITE, CIRCLE_THICKNESS);
    }

    // Ajouter un arc plus épais sur le bord pour définir clairement la zone remplie
    if (sweepAngle > 0) {
        ssd1306Display->drawArc(centerX,
                                centerY,
                                radius,
                                startAngle,
                                endAngle,
                                SSD1306_WHITE,
                                CIRCLE_THICKNESS);
    }
}

float ParameterFocusView::calculateFillRatio() const {
    // Convertir la valeur MIDI (0-127) en ratio (0.0-1.0)
    return static_cast<float>(value_) / 127.0f;
}