#pragma once

#include <memory>

#include "core/utils/Result.hpp"

// Déclarations anticipées
class DependencyContainer;
class IConfiguration;
class IInputSystem;
class IMidiSystem;
class IUISystem;
class EventEnabledMidiOut;
class Ili9341LvglBridge;
class EventBatcher;

/**
 * @brief Application principale du contrôleur MIDI
 */
class MidiControllerApp {
public:
    explicit MidiControllerApp(std::shared_ptr<DependencyContainer> container);
    ~MidiControllerApp();

    // Méthodes principales
    Result<bool, std::string> init();
    void update();

private:
    // Conteneur de dépendances
    std::shared_ptr<DependencyContainer> m_container;

    // Sous-systèmes
    std::shared_ptr<IConfiguration> m_configSystem;
    std::shared_ptr<IInputSystem> m_inputSystem;
    std::shared_ptr<IMidiSystem> m_midiSystem;
    std::shared_ptr<IUISystem> m_uiSystem;
    
    // Bridge LVGL pour le rafraîchissement de l'affichage
    std::shared_ptr<Ili9341LvglBridge> m_lvglBridge;
    
    // Event Batcher pour optimiser les événements UI
    std::unique_ptr<EventBatcher> m_eventBatcher;
};
