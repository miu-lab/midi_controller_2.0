#pragma once

#include <memory>
#include "core/utils/Result.hpp"

// Forward declaration pour éviter les dépendances circulaires
class DependencyContainer;
class IConfiguration;
class IInputSystem;
class IMidiSystem;
class IUISystem;
class UIEventListener;
class EventEnabledMidiOut;

/**
 * @brief Application principale du contrôleur MIDI
 * 
 * Cette classe coordonne les différents sous-systèmes et gère le cycle de vie global
 * de l'application. Elle utilise l'injection de dépendances pour accéder aux sous-systèmes.
 */
class MidiControllerApp {
public:
    /**
     * @brief Constructeur avec conteneur de dépendances
     * @param container Conteneur de dépendances
     */
    explicit MidiControllerApp(std::shared_ptr<DependencyContainer> container);
    
    /**
     * @brief Destructeur
     */
    ~MidiControllerApp();
    
    /**
     * @brief Initialise l'application et tous ses sous-systèmes
     * @return Résultat de l'initialisation
     */
    Result<bool, std::string> init();
    
    /**
     * @brief Met à jour l'application et tous ses sous-systèmes
     */
    void update();
    
private:
    // Conteneur de dépendances
    std::shared_ptr<DependencyContainer> m_container;
    
    // Interfaces des sous-systèmes
    std::shared_ptr<IConfiguration> m_configSystem;
    std::shared_ptr<IInputSystem> m_inputSystem;
    std::shared_ptr<IMidiSystem> m_midiSystem;
    std::shared_ptr<IUISystem> m_uiSystem;
    
    // Écouteur d'événements UI
    std::unique_ptr<UIEventListener> m_uiEventListener;
    
    // Port MIDI avec émission d'événements
    std::shared_ptr<EventEnabledMidiOut> m_eventEnabledMidiOut;
};
