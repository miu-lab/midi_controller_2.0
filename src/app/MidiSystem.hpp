#pragma once
#include "interface_adapters/midi/TeensyUsbMidiOut.hpp"
#include "interface_adapters/midi/BufferedMidiOut.hpp"
#include "interface_adapters/midi/MidiInHandler.hpp"
#include "input/InputRouter.hpp"
#include "storage/ProfileManager.hpp"
#include "app/NavigationConfigService.hpp"

/**
 * @brief Système gérant toutes les fonctionnalités MIDI
 * 
 * Ce système regroupe les composants liés au MIDI : sortie MIDI brute,
 * sortie MIDI bufferisée, traitement des messages MIDI entrants et
 * routage des événements vers les messages MIDI.
 */
class MidiSystem {
public:
    /**
     * @brief Construit un nouveau système MIDI
     * @param profileManager Gestionnaire de profils pour les mappings MIDI
     */
    MidiSystem(ProfileManager& profileManager);
    
    /**
     * @brief Initialise le système MIDI sans gestion de navigation
     */
    void init();
    
    /**
     * @brief Initialise le système MIDI avec gestion de navigation
     * @param navService Service de configuration de navigation
     */
    void init(NavigationConfigService& navService);
    
    /**
     * @brief Met à jour le système MIDI
     * 
     * Traite les messages MIDI entrants et envoie les messages MIDI en attente.
     */
    void update();
    
    /**
     * @brief Obtient la sortie MIDI bufferisée
     * @return Référence à l'implémentation de IMidiOut
     */
    IMidiOut& getMidiOut();
    
private:
    TeensyUsbMidiOut    rawMidiOut_;      // Sortie MIDI USB native
    BufferedMidiOut     bufferedMidiOut_; // Sortie MIDI avec buffer
    MidiInHandler       midiInHandler_;   // Gestion des messages MIDI entrants
    InputRouter         inputRouter_;     // Routage des événements vers MIDI
    ProfileManager&     profileManager_;  // Référence au gestionnaire de profils
};
