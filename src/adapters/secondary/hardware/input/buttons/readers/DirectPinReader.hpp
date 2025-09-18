#pragma once

#include "IPinReader.hpp"
#include "core/domain/types.hpp"
#include <Bounce2.h>

/**
 * @brief Lecteur pour pins connectées directement au MCU
 *
 * Utilise la bibliothèque Bounce2 pour le debounce hardware optimisé.
 */
class DirectPinReader : public IPinReader {
public:
    /**
     * @brief Constructeur avec configuration de pin
     * @param pin Numéro de pin du MCU
     * @param mode Mode de la pin (PULLUP, PULLDOWN, RAW)
     */
    explicit DirectPinReader(uint8_t pin, PinMode mode = PinMode::PULLUP);

    void initialize() override;
    bool readPin() override;
    void update();

private:
    uint8_t pin_;
    PinMode mode_;
    bool initialized_ = false;
    Bounce bounce_;
};