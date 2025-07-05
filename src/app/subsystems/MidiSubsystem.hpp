#pragma once

#include <memory>

#include "config/unified/ControlDefinition.hpp"  // Pour ControlDefinition
#include "adapters/secondary/midi/MidiMapper.hpp"
#include "app/di/DependencyContainer.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IMidiSystem.hpp"
#include "core/ports/output/MidiOutputPort.hpp"
#include "core/utils/Result.hpp"
#include "core/midi/HighPerformanceMidiManager.hpp"
#include "core/memory/EventPoolManager.hpp"

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
     * Cette méthode configure la chaîne de traitement MIDI:
     * TeensyUsbMidiOut -> BufferedMidiOut -> MidiOutputEventAdapter
     *
     * @return Result<bool> Succès ou message d'erreur
     */
    Result<bool> init() override;

    /**
     * @brief Met à jour l'état du sous-système MIDI
     * 
     * Cette méthode effectue les opérations suivantes :
     * 1. Traite les messages MIDI entrants via HighPerformanceMidiManager
     * 2. Met à jour le MidiMapper pour les commandes temporisées
     * 3. Traite les messages MIDI en attente dans BufferedMidiOut
     */
    void update() override;

    /**
     * @brief Envoie un message MIDI Note On
     *
     * @param channel Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @param velocity Vélocité (0-127)
     * @return Result<bool> Succès ou message d'erreur
     */
    Result<bool> sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) override;

    /**
     * @brief Envoie un message MIDI Note Off
     *
     * @param channel Canal MIDI (0-15)
     * @param note Numéro de note (0-127)
     * @return Result<bool> Succès ou message d'erreur
     */
    Result<bool> sendNoteOff(uint8_t channel, uint8_t note) override;

    /**
     * @brief Envoie un message MIDI Control Change
     *
     * @param channel Canal MIDI (0-15)
     * @param controller Numéro de contrôleur (0-127)
     * @param value Valeur (0-127)
     * @return Result<bool> Succès ou message d'erreur
     */
    Result<bool> sendControlChange(uint8_t channel, uint8_t controller,
                                                uint8_t value) override;

    /**
     * @brief Obtient l'interface MidiMapper
     * @return Référence au MidiMapper
     */
    MidiMapper& getMidiMapper() const;
    
    /**
     * @brief Obtient le gestionnaire MIDI haute performance
     * @return Référence au HighPerformanceMidiManager
     */
    HighPerformanceMidiManager& getHighPerformanceMidiManager() const;
    
    /**
     * @brief Traite un message MIDI entrant (appelé depuis ISR ou hardware)
     * @param status Status byte MIDI
     * @param data1 Premier data byte
     * @param data2 Deuxième data byte
     * @return true si traité avec succès
     */
    bool processMidiMessage(uint8_t status, uint8_t data1, uint8_t data2);

private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<IConfiguration> configuration_;
    std::shared_ptr<MidiOutputPort> midiOut_;
    std::unique_ptr<MidiMapper> midiMapper_;
    std::unique_ptr<HighPerformanceMidiManager> highPerformanceMidiManager_;
    std::shared_ptr<CommandManager> commandManager_;
    std::shared_ptr<EventPoolManager> eventPoolManager_;

    bool initialized_ = false;
    
    void loadMidiMappingsFromControlDefinitions() const;
    void setupMidiMappingFromControlDefinition(const ControlDefinition& controlDef) const;
    
    uint8_t extractCCFromInputId(InputId inputId) const;
    bool hasEncoderButton(const ControlDefinition& controlDef) const;
    InputId getEncoderButtonId(InputId encoderId) const;
};