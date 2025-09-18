#include "ButtonFactory.hpp"
#include "adapters/secondary/hardware/input/buttons/readers/DirectPinReader.hpp"
#include "adapters/secondary/hardware/input/buttons/readers/MuxPinReader.hpp"
#include <Arduino.h>

std::unique_ptr<UnifiedButton> ButtonFactory::createButton(const ButtonConfig& config) {
    // Créer le bon lecteur selon le type de GPIO
    auto pinReader = createPinReader(config.gpio);

    if (!pinReader) {
        Serial.print("[ButtonFactory] ERROR: Failed to create pin reader for button ");
        Serial.println(config.id);
        return nullptr;
    }

    // Créer le bouton unifié avec le lecteur
    return std::make_unique<UnifiedButton>(config, std::move(pinReader));
}

std::unique_ptr<IPinReader> ButtonFactory::createPinReader(const GpioPin& gpio) {
    switch (gpio.source) {
        case GpioPin::Source::MCU:
            // Pin directe sur le MCU
            return std::make_unique<DirectPinReader>(gpio.pin, gpio.mode);

        case GpioPin::Source::MUX:
            // Pin via multiplexeur
            if (gpio.pin > 15) {
                Serial.print("[ButtonFactory] ERROR: Invalid MUX channel ");
                Serial.println(gpio.pin);
                return nullptr;
            }
            return std::make_unique<MuxPinReader>(gpio.pin);

        default:
            Serial.print("[ButtonFactory] ERROR: Unsupported GPIO source ");
            Serial.println(static_cast<int>(gpio.source));
            return nullptr;
    }
}