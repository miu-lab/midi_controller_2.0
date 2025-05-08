#pragma once
#include "adapters/secondary/midi/BufferedMidiOut.hpp"
#include "adapters/secondary/midi/MidiInHandler.hpp"
#include "adapters/secondary/midi/MidiMapper.hpp"
#include "adapters/secondary/midi/TeensyUsbMidiOut.hpp"
#include "adapters/secondary/storage/ProfileManager.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "core/domain/commands/CommandManager.hpp"

/**
 * @brief Système gérant toutes les fonctionnalités MIDI
 *
 * Ce système regroupe les composants liés au MIDI : sortie MIDI brute,
 * sortie MIDI bufferisée, traitement des messages MIDI entrants,
 * routage des événements vers les messages MIDI et mapping avec stratégies.
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

    /**
     * @brief Obtient le gestionnaire de commandes MIDI
     * @return Référence au gestionnaire de commandes
     */
    CommandManager& getCommandManager();

    /**
     * @brief Obtient le mapper MIDI qui transforme les événements en commandes
     * @return Référence au mapper MIDI
     */
    MidiMapper& getMidiMapper();

private:
    TeensyUsbMidiOut rawMidiOut_;      // Sortie MIDI USB native
    BufferedMidiOut bufferedMidiOut_;  // Sortie MIDI avec buffer
    MidiInHandler midiInHandler_;      // Gestion des messages MIDI entrants
    CommandManager commandManager_;    // Gestionnaire de commandes MIDI
    MidiMapper midiMapper_;            // Mapper d'événements vers commandes MIDI
    ProfileManager& profileManager_;   // Référence au gestionnaire de profils

    // Méthode d'initialisation commune
    void initSubscriptions();
};
