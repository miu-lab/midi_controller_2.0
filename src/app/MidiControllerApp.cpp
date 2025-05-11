#include "MidiControllerApp.hpp"

#include "app/di/DependencyContainer.hpp"
#include "app/subsystems/ConfigurationSubsystem.hpp"
#include "app/subsystems/InputSubsystem.hpp"
#include "app/subsystems/MidiSubsystem.hpp"
#include "app/subsystems/UISubsystem.hpp"

#ifdef DEBUG
#include <Arduino.h>
#endif

MidiControllerApp::MidiControllerApp(const ApplicationConfiguration& config, 
                                     std::shared_ptr<DependencyContainer> externalContainer) {
    // Si un conteneur externe est fourni, essayer de résoudre les sous-systèmes existants
    if (externalContainer) {
        m_dependencies = externalContainer;
        
        // Ajouter la configuration au conteneur si elle n'existe pas déjà
        if (!m_dependencies->resolve<ApplicationConfiguration>()) {
            m_dependencies->registerDependency<ApplicationConfiguration>(
                std::make_shared<ApplicationConfiguration>(config));
        }
        
        // Utiliser les interfaces déjà injectées
        auto configInterface = m_dependencies->resolve<IConfiguration>();
        auto inputInterface = m_dependencies->resolve<IInputSystem>();
        auto midiInterface = m_dependencies->resolve<IMidiSystem>();
        auto uiInterface = m_dependencies->resolve<IUISystem>();
        
        // Ces interfaces sont injectées par les tests
        if (configInterface && inputInterface && midiInterface && uiInterface) {
            // Nous n'avons pas besoin de créer nos propres sous-systèmes
            // Nous utiliserons directement les interfaces injectées lors de init() et update()
            return;
        }
    }
    
    // Si nous sommes ici, soit nous n'avons pas de conteneur externe,
    // soit il ne contient pas toutes les interfaces nécessaires
    
    // Créer un nouveau conteneur si nécessaire
    if (!m_dependencies) {
        m_dependencies = std::make_shared<DependencyContainer>();
    }
    
    // Enregistrer la configuration
    if (!m_dependencies->resolve<ApplicationConfiguration>()) {
        m_dependencies->registerDependency<ApplicationConfiguration>(
            std::make_shared<ApplicationConfiguration>(config));
    }
    
    // Créer et enregistrer les sous-systèmes
    m_configSystem = std::make_shared<ConfigurationSubsystem>(m_dependencies);
    m_inputSystem = std::make_shared<InputSubsystem>(m_dependencies);
    m_midiSystem = std::make_shared<MidiSubsystem>(m_dependencies);
    m_uiSystem = std::make_shared<UISubsystem>(m_dependencies);
    
    // Enregistrer les sous-systèmes comme implémentations d'interfaces
    m_dependencies->registerDependency<IConfiguration>(m_configSystem);
    m_dependencies->registerDependency<IInputSystem>(m_inputSystem);
    m_dependencies->registerDependency<IMidiSystem>(m_midiSystem);
    m_dependencies->registerDependency<IUISystem>(m_uiSystem);
}

MidiControllerApp::~MidiControllerApp() {
    // Nettoyer les ressources
    m_uiSystem.reset();
    m_midiSystem.reset();
    m_inputSystem.reset();
    m_configSystem.reset();
    m_dependencies.reset();
}

Result<bool, std::string> MidiControllerApp::init() {
    // Obtenir les interfaces pour l'initialisation
    auto configInterface = m_dependencies->resolve<IConfiguration>();
    auto inputInterface = m_dependencies->resolve<IInputSystem>();
    auto midiInterface = m_dependencies->resolve<IMidiSystem>();
    auto uiInterface = m_dependencies->resolve<IUISystem>();
    
    // Vérifier que toutes les interfaces sont disponibles
    if (!configInterface || !inputInterface || !midiInterface || !uiInterface) {
        return Result<bool, std::string>::error("Certaines interfaces nécessaires sont manquantes");
    }
    
    // 1. Configuration (indépendant)
#ifdef DEBUG
    Serial.println(F("MidiControllerApp: Initialisation du sous-système de configuration"));
#endif
    auto configResult = configInterface->init();
    if (configResult.isError()) {
        return Result<bool, std::string>::error(
            "Échec de l'initialisation du sous-système de configuration: " + 
            *(configResult.error()));
    }
    
    // 2. Input (dépend de Configuration)
#ifdef DEBUG
    Serial.println(F("MidiControllerApp: Initialisation du sous-système d'entrée"));
#endif
    auto inputResult = inputInterface->init();
    if (inputResult.isError()) {
        return Result<bool, std::string>::error(
            "Échec de l'initialisation du sous-système d'entrée: " + 
            *(inputResult.error()));
    }
    
    // 3. MIDI (peut dépendre de Configuration)
#ifdef DEBUG
    Serial.println(F("MidiControllerApp: Initialisation du sous-système MIDI"));
#endif
    auto midiResult = midiInterface->init();
    if (midiResult.isError()) {
        return Result<bool, std::string>::error(
            "Échec de l'initialisation du sous-système MIDI: " + 
            *(midiResult.error()));
    }
    
    // 4. UI (peut dépendre de tous les autres)
#ifdef DEBUG
    Serial.println(F("MidiControllerApp: Initialisation du sous-système d'interface utilisateur"));
#endif
    auto uiResult = uiInterface->init(/* enableFullUI */ true);
    if (uiResult.isError()) {
        return Result<bool, std::string>::error(
            "Échec de l'initialisation du sous-système d'interface utilisateur: " + 
            *(uiResult.error()));
    }
    
    return Result<bool, std::string>::success(true);
}

void MidiControllerApp::update() {
    // Obtenir les interfaces pour la mise à jour
    auto inputInterface = m_dependencies->resolve<IInputSystem>();
    auto midiInterface = m_dependencies->resolve<IMidiSystem>();
    auto uiInterface = m_dependencies->resolve<IUISystem>();
    
    // Mise à jour des sous-systèmes dans l'ordre logique
    if (inputInterface) inputInterface->update();
    if (midiInterface) midiInterface->update();
    if (uiInterface) uiInterface->update();
}

// Implémentation de l'API publique
void MidiControllerApp::setControlForNavigation(ControlId id, bool isNavigation) {
    auto configInterface = m_dependencies->resolve<IConfiguration>();
    if (configInterface) {
        configInterface->setControlForNavigation(id, isNavigation);
    }
}

bool MidiControllerApp::isNavigationControl(ControlId id) const {
    auto configInterface = m_dependencies->resolve<IConfiguration>();
    if (configInterface) {
        return configInterface->isNavigationControl(id);
    }
    return false;
}

// Implémentation des accesseurs d'interfaces
std::shared_ptr<IConfiguration> MidiControllerApp::getConfiguration() const {
    return m_dependencies->resolve<IConfiguration>();
}

std::shared_ptr<IInputSystem> MidiControllerApp::getInputInterface() const {
    return m_dependencies->resolve<IInputSystem>();
}

std::shared_ptr<IMidiSystem> MidiControllerApp::getMidiInterface() const {
    return m_dependencies->resolve<IMidiSystem>();
}

std::shared_ptr<IUISystem> MidiControllerApp::getUIInterface() const {
    return m_dependencies->resolve<IUISystem>();
}