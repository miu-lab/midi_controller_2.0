#include "MidiControllerApp2.hpp"
#include "app/di/DependencyContainer.hpp"
#include "app/subsystems/ConfigurationSubsystem.hpp"
#include "app/subsystems/InputSubsystem.hpp"
#include "app/subsystems/MidiSubsystem.hpp"
#include "app/subsystems/UISubsystem.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IInputSystem.hpp"
#include "core/domain/interfaces/IMidiSystem.hpp"
#include "core/domain/interfaces/IUISystem.hpp"

#ifdef DEBUG
#include <Arduino.h>
#endif

MidiControllerApp2::MidiControllerApp2(const ApplicationConfiguration& config) {
    // Initialisation du conteneur de dépendances
    m_dependencies = std::make_shared<DependencyContainer>();
    
    // Enregistrement des dépendances globales
    m_dependencies->registerDependency<ApplicationConfiguration>(
        std::make_shared<ApplicationConfiguration>(config)
    );
    
    // Création des sous-systèmes
    m_configSystem = std::make_shared<ConfigurationSubsystem>(m_dependencies);
    m_inputSystem = std::make_shared<InputSubsystem>(m_dependencies);
    m_midiSystem = std::make_shared<MidiSubsystem>(m_dependencies);
    m_uiSystem = std::make_shared<UISubsystem>(m_dependencies);
    
    // Enregistrement des sous-systèmes dans le conteneur
    m_dependencies->registerDependency<ConfigurationSubsystem>(m_configSystem);
    m_dependencies->registerDependency<InputSubsystem>(m_inputSystem);
    m_dependencies->registerDependency<MidiSubsystem>(m_midiSystem);
    m_dependencies->registerDependency<UISubsystem>(m_uiSystem);
    
    // Enregistrement des interfaces
    m_dependencies->registerDependency<IConfiguration>(m_configSystem);
    m_dependencies->registerDependency<IInputSystem>(m_inputSystem);
    m_dependencies->registerDependency<IMidiSystem>(m_midiSystem);
    m_dependencies->registerDependency<IUISystem>(m_uiSystem);
}

MidiControllerApp2::~MidiControllerApp2() {
    // L'ordre de destruction est important
    // On reset explicitement dans l'ordre inverse de création
    m_uiSystem.reset();
    m_midiSystem.reset();
    m_inputSystem.reset();
    m_configSystem.reset();
    
    // En dernier, on libère le conteneur
    m_dependencies.reset();
}

Result<bool, std::string> MidiControllerApp2::init() {
    // L'initialisation doit suivre un ordre précis en fonction des dépendances
    
    // 1. Configuration (indépendant)
#ifdef DEBUG
    Serial.println(F("MidiControllerApp2: Initialisation du sous-système de configuration"));
#endif
    auto configResult = m_configSystem->init();
    if (configResult.isError()) {
        return Result<bool, std::string>::error("Échec de l'initialisation du sous-système de configuration: " + 
                                             *(configResult.error()));
    }
    
    // 2. Input (dépend de Configuration)
#ifdef DEBUG
    Serial.println(F("MidiControllerApp2: Initialisation du sous-système d'entrée"));
#endif
    auto inputResult = m_inputSystem->init();
    if (inputResult.isError()) {
        return Result<bool, std::string>::error("Échec de l'initialisation du sous-système d'entrée: " + 
                                            *(inputResult.error()));
    }
    
    // 3. MIDI (peut dépendre de Configuration)
#ifdef DEBUG
    Serial.println(F("MidiControllerApp2: Initialisation du sous-système MIDI"));
#endif
    auto midiResult = m_midiSystem->init();
    if (midiResult.isError()) {
        return Result<bool, std::string>::error("Échec de l'initialisation du sous-système MIDI: " + 
                                           *(midiResult.error()));
    }
    
    // 4. UI (peut dépendre de tous les autres)
#ifdef DEBUG
    Serial.println(F("MidiControllerApp2: Initialisation du sous-système d'interface utilisateur"));
#endif
    auto uiResult = m_uiSystem->init(/* enableFullUI */ true);
    if (uiResult.isError()) {
        return Result<bool, std::string>::error("Échec de l'initialisation du sous-système d'interface utilisateur: " + 
                                           *(uiResult.error()));
    }
    
    return Result<bool, std::string>::success(true);
}

void MidiControllerApp2::update() {
    // Mise à jour des sous-systèmes dans l'ordre logique
    m_inputSystem->update();  // Capture des entrées utilisateur
    m_midiSystem->update();   // Traitement MIDI (peut dépendre des entrées)
    m_uiSystem->update();     // Mise à jour de l'UI (reflète l'état des autres systèmes)
}

// Implémentation de l'API publique
void MidiControllerApp2::setControlForNavigation(ControlId id, bool isNavigation) {
    m_configSystem->setControlForNavigation(id, isNavigation);
}

bool MidiControllerApp2::isNavigationControl(ControlId id) const {
    return m_configSystem->isNavigationControl(id);
}