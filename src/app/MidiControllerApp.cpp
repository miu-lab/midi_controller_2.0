#include "MidiControllerApp.hpp"

#include "adapters/primary/ui/ViewManager.hpp"
#include "adapters/secondary/midi/EventEnabledMidiOut.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/debug/DebugMacros.hpp"
#include "config/debug/TaskSchedulerConfig.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IInputSystem.hpp"
#include "core/domain/interfaces/IMidiSystem.hpp"
#include "core/domain/interfaces/IUISystem.hpp"
#include "core/ports/output/MidiOutputPort.hpp"

MidiControllerApp::MidiControllerApp(std::shared_ptr<DependencyContainer> container)
    : m_container(container) {}

MidiControllerApp::~MidiControllerApp() {
    // Libération des ressources dans l'ordre inverse
    m_uiSystem.reset();
    m_midiSystem.reset();
    m_inputSystem.reset();
    m_configSystem.reset();
}

Result<bool, std::string> MidiControllerApp::init() {
    // 1. Obtention des références aux sous-systèmes
    m_configSystem = m_container->resolve<IConfiguration>();
    m_inputSystem = m_container->resolve<IInputSystem>();
    m_midiSystem = m_container->resolve<IMidiSystem>();
    m_uiSystem = m_container->resolve<IUISystem>();

    if (!m_configSystem || !m_inputSystem || !m_midiSystem || !m_uiSystem) {
        return Result<bool, std::string>::error("Sous-systèmes manquants");
    }

    // Le décorateur MIDI pour les événements est maintenant créé et géré par MidiSubsystem

    return Result<bool, std::string>::success(true);
}

void MidiControllerApp::update() {
    // Mettre à jour le sous-système d'entrée
    if (m_inputSystem) {
        m_inputSystem->update();
    }
}
