#pragma once

#include <cstdint>
#include "core/utils/Result.hpp"

/**
 * @brief Interface pour le système MIDI
 * 
 * Cette interface définit les méthodes que tous les systèmes MIDI doivent implémenter.
 */
class IMidiSystem {
public:
    virtual ~IMidiSystem() = default;
    
    /**
     * @brief Initialise le système MIDI
     * 
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    virtual Result<bool, std::string> init() = 0;
    
    /**
     * @brief Met à jour l'état du système MIDI
     */
    virtual void update() = 0;
    
    /**
     * @brief Envoie un message MIDI Note On
     * 
     * @param channel Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @param velocity Vélocité (0-127)
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    virtual Result<bool, std::string> sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) = 0;
    
    /**
     * @brief Envoie un message MIDI Note Off
     * 
     * @param channel Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    virtual Result<bool, std::string> sendNoteOff(uint8_t channel, uint8_t note) = 0;
    
    /**
     * @brief Envoie un message MIDI Control Change
     * 
     * @param channel Canal MIDI (0-15)
     * @param controller Numéro de contrôleur (0-127)
     * @param value Valeur (0-127)
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    virtual Result<bool, std::string> sendControlChange(uint8_t channel, uint8_t controller, uint8_t value) = 0;
};