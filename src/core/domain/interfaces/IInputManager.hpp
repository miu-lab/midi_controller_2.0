#pragma once

#include <memory>
#include <vector>

#include "config/unified/ControlDefinition.hpp"
#include "core/utils/Result.hpp"

// Forward declarations
class InputController;

/**
 * @brief Interface pour la gestion centralisée des entrées utilisateur
 * 
 * Cette interface définit les opérations nécessaires pour gérer
 * les entrées utilisateur dans le système MIDI.
 */
class IInputManager {
public:
    /**
     * @brief Configuration pour InputManager
     */
    struct ManagerConfig {
        bool enableEncoders;
        bool enableButtons;
        bool enableEventProcessing;
        
        ManagerConfig() : enableEncoders(true), enableButtons(true), enableEventProcessing(true) {}
    };

    /**
     * @brief Destructeur virtuel
     */
    virtual ~IInputManager() = default;

    /**
     * @brief Initialise le gestionnaire avec les définitions de contrôles
     * @param controlDefinitions Définitions des contrôles à gérer
     * @param inputController Contrôleur d'entrée pour les événements
     * @return Result<bool> Succès ou erreur
     */
    virtual Result<bool> initialize(const std::vector<ControlDefinition>& controlDefinitions,
                                   std::shared_ptr<InputController> inputController) = 0;

    /**
     * @brief Met à jour tous les composants de gestion des entrées
     */
    virtual void update() = 0;

    /**
     * @brief Reconfigure les entrées avec nouvelles définitions
     * @param controlDefinitions Nouvelles définitions de contrôles
     * @return Result<bool> Succès ou erreur
     */
    virtual Result<bool> reconfigure(const std::vector<ControlDefinition>& controlDefinitions) = 0;

    /**
     * @brief Vérifie si le gestionnaire est opérationnel
     * @return true si le gestionnaire est initialisé et opérationnel
     */
    virtual bool isOperational() const = 0;
};