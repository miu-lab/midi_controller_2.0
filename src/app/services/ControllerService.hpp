#pragma once

#include "adapters/primary/ui/ViewManager.hpp"
#include "adapters/secondary/midi/MidiMapper.hpp"
#include "core/controllers/InputController.hpp"
#include "core/controllers/MenuController.hpp"
#include "core/controllers/ProfileController.hpp"
#include "core/controllers/UIController.hpp"
#include "core/domain/IMidiOut.hpp"
#include "core/domain/IProfileManager.hpp"
#include "core/domain/commands/CommandManager.hpp"

/**
 * @brief Service coordonnant les différents contrôleurs
 *
 * Ce service instancie et coordonne les différents contrôleurs
 * spécialisés (menu, MIDI, profil).
 */
class ControllerService {
public:
    /**
     * @brief Constructeur
     * @param viewManager Gestionnaire de vues
     * @param midiOut Interface de sortie MIDI
     * @param profileManager Gestionnaire de profils
     */
    ControllerService(ViewManager& viewManager, IMidiOut& midiOut, IProfileManager& profileManager);

    /**
     * @brief Initialise le service
     */
    void init();

    /**
     * @brief Met à jour le service
     */
    void update();

    /**
     * @brief Obtient le contrôleur de menu
     * @return Référence au contrôleur de menu
     */
    MenuController& getMenuController();

    /**
     * @brief Obtient le contrôleur MIDI
     * @return Référence au contrôleur MIDI
     */
    MidiMapper& getMidiMapper();

    /**
     * @brief Obtient le contrôleur de profil
     * @return Référence au contrôleur de profil
     */
    ProfileController& getProfileController();
    
    /**
     * @brief Obtient le contrôleur d'interface utilisateur
     * @return Référence au contrôleur d'interface utilisateur
     */
    UIController& getUIController();
    
    /**
     * @brief Obtient le contrôleur d'entrées
     * @return Référence au contrôleur d'entrées
     */
    InputController& getInputController();

private:
    ViewManager& viewManager_;
    IMidiOut& midiOut_;
    IProfileManager& profileManager_;

    CommandManager commandManager_;
    
    // Contrôleurs principaux
    MenuController menuController_;
    MidiMapper midiMapper_;
    ProfileController profileController_;
    UIController* uiController_;
    InputController* inputController_;

    /**
     * @brief Initialise les mappings MIDI
     */
    void initializeMidiMappings();
};
