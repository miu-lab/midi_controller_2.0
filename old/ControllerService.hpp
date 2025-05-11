#pragma once

// Forward declarations
class ControllerServiceEventListener;
class DependencyContainer;

#include <memory>

#include "adapters/primary/ui/ViewManager.hpp"
#include "adapters/secondary/midi/MidiMapper.hpp"
#include "core/controllers/InputController.hpp"
#include "core/controllers/MenuController.hpp"
#include "core/controllers/ProfileController.hpp"
#include "core/controllers/UIController.hpp"
#include "core/domain/IMidiOut.hpp"
#include "core/domain/IProfileManager.hpp"
#include "core/domain/commands/CommandManager.hpp"
#include "core/domain/events/EventSystem.hpp"

/**
 * @brief Service coordonnant les différents contrôleurs
 *
 * Ce service instancie et coordonne les différents contrôleurs
 * spécialisés (menu, MIDI, profil).
 */
class ControllerService {
public:
    /**
     * @brief Constructeur original
     * @param viewManager Gestionnaire de vues
     * @param midiOut Interface de sortie MIDI
     * @param profileManager Gestionnaire de profils
     */
    ControllerService(ViewManager& viewManager, IMidiOut& midiOut, IProfileManager& profileManager);
    
    /**
     * @brief Nouveau constructeur avec injection de dépendances
     * @param container Conteneur de dépendances
     */
    explicit ControllerService(std::shared_ptr<DependencyContainer> container);

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
    // Conteneur de dépendances (peut être nullptr pour l'ancien constructeur)
    std::shared_ptr<DependencyContainer> container_;
    
    // Dépendances directes pour l'ancien constructeur
    ViewManager& viewManager_;
    IMidiOut& midiOut_;
    IProfileManager& profileManager_;
    
    // Flag pour savoir si on utilise le container
    bool usingContainer_ = false;

    CommandManager commandManager_;
    
    // Contrôleurs principaux
    MenuController menuController_;
    MidiMapper midiMapper_;
    ProfileController profileController_;
    UIController* uiController_;            // Pointeur vers le contrôleur UI
    InputController* inputController_;      // Pointeur vers le contrôleur d'entrées
    
    // Écouteur d'événements (nous en sommes propriétaires)
    std::shared_ptr<ControllerServiceEventListener> eventListener_;

    /**
     * @brief Initialise les mappings MIDI
     */
    void initializeMidiMappings();
    
    /**
     * @brief Initialise les dépendances (UIController et InputController)
     */
    void initializeDependencies();
};
