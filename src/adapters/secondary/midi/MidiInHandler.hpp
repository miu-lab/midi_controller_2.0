#pragma once
#include <functional>
#include "config/ETLConfig.hpp"
#include "core/domain/types.hpp"

/**
 * @brief Gestionnaire pour les messages MIDI entrants
 */
class MidiInHandler {
public:
    // Types de callbacks
    using CcCallback = std::function<void(MidiChannel, MidiCC, uint8_t)>;
    using NoteCallback = std::function<void(MidiChannel, MidiNote, uint8_t)>;

    MidiInHandler();

    // Méthodes pour enregistrer des callbacks
    void onControlChange(CcCallback callback);
    void onNoteOn(NoteCallback callback);
    void onNoteOff(NoteCallback callback);

    // Méthode à appeler régulièrement pour traiter les messages MIDI entrants
    void update();

private:
    // Callbacks avec tailles fixes ETL (évite allocations dynamiques)
    ETLConfig::MidiCallbackVector<CcCallback> ccCallbacks_;
    ETLConfig::MidiCallbackVector<NoteCallback> noteOnCallbacks_;
    ETLConfig::MidiCallbackVector<NoteCallback> noteOffCallbacks_;

    // Méthodes de traitement des messages
    void handleControlChange(MidiChannel channel, MidiCC cc, uint8_t value);
    void handleNoteOn(MidiChannel channel, MidiNote note, uint8_t velocity);
    void handleNoteOff(MidiChannel channel, MidiNote note, uint8_t velocity);
};
