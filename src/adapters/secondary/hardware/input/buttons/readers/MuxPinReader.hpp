#pragma once

#include <cstdint>
#include "IPinReader.hpp"

/**
 * @brief Lecteur pour pins connectées via le multiplexeur CD74HC4067
 *
 * Gère automatiquement la sélection du canal et la lecture via le MultiplexerManager.
 */
class MuxPinReader : public IPinReader {
public:
    /**
     * @brief Constructeur avec numéro de canal
     * @param channel Canal du multiplexeur (0-15)
     */
    explicit MuxPinReader(uint8_t channel);

    void initialize() override;
    bool readPin() override;

    /**
     * @brief Obtient le canal configuré
     */
    uint8_t getChannel() const { return channel_; }

private:
    uint8_t channel_;
    bool initialized_ = false;
};