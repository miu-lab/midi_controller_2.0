#include "MidiControllerApp.hpp"

#include "app/di/DependencyContainer.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IInputSystem.hpp"
#include "core/domain/interfaces/IMidiSystem.hpp"
#include "core/domain/interfaces/IUISystem.hpp"

#ifdef DEBUG
#include <Arduino.h>
#endif

MidiControllerApp::MidiControllerApp(std::shared_ptr<DependencyContainer> container)
    : m_container(container) {
}

MidiControllerApp::~MidiControllerApp() {
    // Nettoyer les ressources dans l'ordre inverse de leur création
    m_uiSystem.reset();
    m_midiSystem.reset();
    m_inputSystem.reset();
    m_configSystem.reset();
}

Result<bool, std::string> MidiControllerApp::init() {
    // Résoudre les interfaces des sous-systèmes
    m_configSystem = m_container->resolve<IConfiguration>();
    if (!m_configSystem) {
        return Result<bool, std::string>::error("Failed to resolve IConfiguration");
    }
    
    m_inputSystem = m_container->resolve<IInputSystem>();
    if (!m_inputSystem) {
        return Result<bool, std::string>::error("Failed to resolve IInputSystem");
    }
    
    m_midiSystem = m_container->resolve<IMidiSystem>();
    if (!m_midiSystem) {
        return Result<bool, std::string>::error("Failed to resolve IMidiSystem");
    }
    
    m_uiSystem = m_container->resolve<IUISystem>();
    if (!m_uiSystem) {
        return Result<bool, std::string>::error("Failed to resolve IUISystem");
    }
    
    // Initialiser les sous-systèmes dans l'ordre de dépendance
    
    // 1. Configuration (indépendant)
    #ifdef DEBUG
    Serial.println(F("MidiControllerApp: Initialisation du sous-système de configuration"));
    #endif
    auto configResult = m_configSystem->init();
    if (configResult.isError()) {
        return Result<bool, std::string>::error(
            std::string("Failed to initialize configuration subsystem: ") + 
            *(configResult.error()));
    }
    
    // 2. Input (dépend de Configuration)
    #ifdef DEBUG
    Serial.println(F("MidiControllerApp: Initialisation du sous-système d'entrée"));
    #endif
    auto inputResult = m_inputSystem->init();
    if (inputResult.isError()) {
        return Result<bool, std::string>::error(
            std::string("Failed to initialize input subsystem: ") + 
            *(inputResult.error()));
    }
    
    // 3. MIDI (peut dépendre de Configuration)
    #ifdef DEBUG
    Serial.println(F("MidiControllerApp: Initialisation du sous-système MIDI"));
    #endif
    auto midiResult = m_midiSystem->init();
    if (midiResult.isError()) {
        return Result<bool, std::string>::error(
            std::string("Failed to initialize MIDI subsystem: ") + 
            *(midiResult.error()));
    }
    
    // 4. UI (peut dépendre de tous les autres)
    #ifdef DEBUG
    Serial.println(F("MidiControllerApp: Initialisation du sous-système d'interface utilisateur"));
    #endif
    auto uiResult = m_uiSystem->init(true);  // true = enable full UI
    if (uiResult.isError()) {
        return Result<bool, std::string>::error(
            std::string("Failed to initialize UI subsystem: ") + 
            *(uiResult.error()));
    }
    
    return Result<bool, std::string>::success(true);
}

void MidiControllerApp::update() {
    // Mettre à jour les sous-systèmes dans l'ordre logique
    if (m_inputSystem) m_inputSystem->update();
    if (m_midiSystem) m_midiSystem->update();
    if (m_uiSystem) m_uiSystem->update();
}