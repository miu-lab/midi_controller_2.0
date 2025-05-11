#pragma once

#include <memory>
#include <iostream>
#include "core/domain/interfaces/IMidiSystem.hpp"
#include "app/di/DependencyContainer.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/IMidiOut.hpp"
#include "adapters/secondary/midi/DummyUsbMidi.hpp"
#include "adapters/secondary/midi/TeensyUsbMidi.hpp"
#include "core/utils/Result.hpp"

/**
 * @brief Sous-système MIDI
 * 
 * Cette classe implémente l'interface IMidiSystem et gère toutes
 * les communications MIDI.
 */
class MidiSubsystem : public IMidiSystem {
public:
    /**
     * @brief Constructeur avec injection de dépendances
     * @param container Conteneur de dépendances
     */
    explicit MidiSubsystem(std::shared_ptr<DependencyContainer> container);
    
    /**
     * @brief Destructeur par défaut
     */
    ~MidiSubsystem() override = default;
    
    /**
     * @brief Initialise le sous-système MIDI
     * 
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> init() override;
    
    /**
     * @brief Met à jour l'état du sous-système MIDI
     */
    void update() override;
    
    /**
     * @brief Envoie un message MIDI Note On
     * 
     * @param channel Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @param velocity Vélocité (0-127)
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override;
    
    /**
     * @brief Envoie un message MIDI Note Off
     * 
     * @param channel Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> sendNoteOff(uint8_t channel, uint8_t note) override;
    
    /**
     * @brief Envoie un message MIDI Control Change
     * 
     * @param channel Canal MIDI (0-15)
     * @param controller Numéro de contrôleur (0-127)
     * @param value Valeur (0-127)
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    Result<bool, std::string> sendControlChange(uint8_t channel, uint8_t controller, uint8_t value) override;
    
private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<IConfiguration> configuration_;
    std::shared_ptr<IMidiOut> midiOut_;
    
    bool initialized_ = false;
};