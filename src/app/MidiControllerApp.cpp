#include "MidiControllerApp.hpp"

#include "app/di/DependencyContainer.hpp"
#include "config/debug/DebugMacros.hpp"
#include "core/TaskScheduler.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IInputSystem.hpp"
#include "core/domain/interfaces/IMidiSystem.hpp"
#include "core/domain/interfaces/IUISystem.hpp"


MidiControllerApp::MidiControllerApp(std::shared_ptr<DependencyContainer> container)
    : m_container(container) {
    m_scheduler = m_container->resolve<TaskScheduler>();
}

MidiControllerApp::~MidiControllerApp() {
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
    // Mettre à jour le scheduler de tâches en premier
    if (m_scheduler) {
        m_scheduler->update();
    }
}
