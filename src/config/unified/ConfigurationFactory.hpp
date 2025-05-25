#pragma once

#include <memory>
#include "config/unified/UnifiedConfiguration.hpp"

/**
 * @brief Factory pour créer la configuration complète du contrôleur MIDI
 * 
 * Démontre comment utiliser le nouveau système unifié pour définir
 * tous les contrôles de manière concise et cohérente.
 */
class ConfigurationFactory {
public:
    /**
     * @brief Crée la configuration par défaut du contrôleur MIDI
     * Cette configuration correspond exactement à l'ancienne pour assurer la compatibilité
     */
    static std::unique_ptr<UnifiedConfiguration> createDefaultConfiguration();

private:
    // Helpers pour mapper les pins selon la configuration actuelle
    static uint8_t getEncoderPinA(int encoderNum);
    static uint8_t getEncoderPinB(int encoderNum);
    static uint8_t getEncoderButtonPin(int encoderNum);
};
