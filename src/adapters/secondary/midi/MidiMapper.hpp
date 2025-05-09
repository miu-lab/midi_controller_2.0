#pragma once

#include <memory>
#include <unordered_map>

#include "core/domain/IMidiOut.hpp"
#include "core/domain/commands/CommandManager.hpp"
#include "core/domain/commands/midi/SendMidiCCCommand.hpp"
#include "core/domain/commands/midi/SendMidiNoteCommand.hpp"
#include "core/domain/strategies/MidiMappingStrategy.hpp"
#include "core/domain/types.hpp"
#include "config/MappingConfiguration.hpp"

/**
 * @brief Mapper qui gère la transformation des événements en commandes MIDI
 *
 * Cette classe utilise le Command Pattern et le Strategy Pattern
 * pour transformer les événements d'entrée en commandes MIDI.
 */
class MidiMapper {
public:
    /**
     * @brief Constructeur
     * @param midiOut Interface de sortie MIDI
     * @param commandManager Gestionnaire de commandes
     */
    MidiMapper(IMidiOut& midiOut, CommandManager& commandManager);

    /**
     * @brief Définit le mapping pour un contrôle
     * @param controlId ID du contrôle
     * @param midiControl Paramètres de contrôle MIDI
     * @param strategy Stratégie de mapping à utiliser
     */
    void setMapping(ControlId controlId, const MidiControl& midiControl,
                    std::unique_ptr<IMidiMappingStrategy> strategy);

    /**
     * @brief Supprime le mapping pour un contrôle
     * @param controlId ID du contrôle
     * @return true si le mapping a été supprimé, false s'il n'existait pas
     */
    bool removeMapping(ControlId controlId);

    /**
     * @brief Vérifie si un contrôle a un mapping
     * @param controlId ID du contrôle
     * @return true si le contrôle a un mapping, false sinon
     */
    bool hasMapping(ControlId controlId) const;

    /**
     * @brief Obtient le mapping MIDI pour un contrôle
     * @param controlId ID du contrôle
     * @return Paramètres du contrôle MIDI, ou une valeur par défaut si non trouvé
     */
    const MidiControl& getMidiControl(ControlId controlId) const;

    /**
     * @brief Traite un changement d'encodeur
     * @param encoderId ID de l'encodeur
     * @param position Position absolue de l'encodeur
     */
    void processEncoderChange(EncoderId encoderId, int32_t position);

    /**
     * @brief Traite un appui sur un bouton d'encodeur
     * @param encoderId ID de l'encodeur
     * @param pressed true si le bouton est pressé, false s'il est relâché
     */
    void processEncoderButton(EncoderId encoderId, bool pressed);

    /**
     * @brief Traite un appui sur un bouton
     * @param buttonId ID du bouton
     * @param pressed true si le bouton est pressé, false s'il est relâché
     */
    void processButtonPress(ButtonId buttonId, bool pressed);

    /**
     * @brief Met à jour l'état des commandes
     * Appelé régulièrement pour traiter les commandes temporisées
     */
    void update();

private:
    // Structure pour stocker les informations de mapping
    struct MappingInfo {
        MidiControl control;
        std::unique_ptr<IMidiMappingStrategy> strategy;
        uint8_t lastMidiValue;
        int32_t lastEncoderPosition;
        int32_t midiOffset = 0;   // Offset pour le référentiel flottant
    };

    IMidiOut& midiOut_;
    CommandManager& commandManager_;
    std::unordered_map<ControlId, MappingInfo> mappings_;
    std::unordered_map<ControlId, std::unique_ptr<SendMidiNoteCommand>> activeNotes_;
    
    MidiControl defaultControl_;  // Contrôle par défaut retourné si non trouvé
    
    // Vérifie si un contrôle est dédié à la navigation en le consultant auprès de MappingConfiguration
    bool isNavigationControl(ControlId controlId) const;
};
