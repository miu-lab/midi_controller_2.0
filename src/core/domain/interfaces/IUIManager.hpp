#pragma once

#include <memory>
#include <string>

#include "core/utils/Result.hpp"

// Forward declarations
namespace MidiController::Events {
    class IEventBus;
}

/**
 * @brief Interface pour la gestion centralisée du système UI
 * 
 * Cette interface définit les opérations nécessaires pour gérer
 * l'interface utilisateur du contrôleur MIDI.
 */
class IUIManager {
public:
    /**
     * @brief Configuration pour le gestionnaire UI
     */
    struct UIConfig {
        bool enableFullUI;
        bool enableEventProcessing;
        bool enableDisplayRefresh;
        
        UIConfig() 
            : enableFullUI(false)
            , enableEventProcessing(true) 
            , enableDisplayRefresh(true) {}
    };

    /**
     * @brief Destructeur virtuel
     */
    virtual ~IUIManager() = default;

    /**
     * @brief Initialise le gestionnaire UI avec un bus d'événements
     * @param eventBus Bus d'événements unifié
     * @return Result indiquant le succès ou l'erreur
     */
    virtual Result<bool> initialize(std::shared_ptr<MidiController::Events::IEventBus> eventBus) = 0;

    /**
     * @brief Met à jour tous les composants UI dans le bon ordre
     */
    virtual void update() = 0;

    /**
     * @brief Affiche un message modal
     * @param message Message à afficher
     * @return Result indiquant le succès ou l'erreur
     */
    virtual Result<bool> showMessage(const std::string& message) = 0;

    /**
     * @brief Efface l'affichage et ferme les modals
     * @return Result indiquant le succès ou l'erreur
     */
    virtual Result<bool> clearDisplay() = 0;

    /**
     * @brief Vérifie si le gestionnaire est initialisé
     * @return true si initialisé
     */
    virtual bool isInitialized() const = 0;

    /**
     * @brief Vérifie si le gestionnaire est opérationnel
     * @return true si tous les composants sont présents et l'UI activée
     */
    virtual bool isOperational() const = 0;
};