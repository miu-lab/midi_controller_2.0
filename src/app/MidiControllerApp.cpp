// app/MidiControllerApp.cpp
#include "app/MidiControllerApp.hpp"
#include <Arduino.h>
#include "adapters/primary/Scheduler.hpp"
#include "core/domain/events/InputEvent.hpp"

MidiControllerApp::MidiControllerApp(const ApplicationConfiguration& appConfig)
    // Service de configuration
    : configService_(appConfig)
    // Gestion des profils et navigation
    , profileManager_()
    // Systèmes
    , inputSystem_()
    , midiSystem_(profileManager_)
{
    // Initialiser le ServiceLocator avec la configuration
    ServiceLocator::initialize(appConfig);
    
    // Enregistrer les services dans le ServiceLocator
    ServiceLocator::registerNavigationConfigService(&navigationConfig_);
    ServiceLocator::registerProfileManager(&profileManager_);
    ServiceLocator::registerMidiSystem(&midiSystem_);
    ServiceLocator::registerInputSystem(&inputSystem_);
    ServiceLocator::registerUiEventService(&uiService_);
    ServiceLocator::registerConfigurationService(&configService_);
}

void MidiControllerApp::setControlForNavigation(ControlId id, bool isNavigation) {
    navigationConfig_.setControlForNavigation(id, isNavigation);
}

bool MidiControllerApp::isNavigationControl(ControlId id) const {
    return navigationConfig_.isNavigationControl(id);
}

void MidiControllerApp::begin()
{
    // Ne pas attendre le port série pour ne pas bloquer le MIDI
    // Optionnel: ajouter un timeout pour permettre une connexion série si disponible rapidement
    // unsigned long startTime = millis();
    // while (!Serial && (millis() - startTime < 1000)) { /* attend jusqu'à 1 seconde */ }
    
    // 1) Appliquer les configurations aux différents services
    configService_.applyConfigurations(profileManager_, navigationConfig_);
    
    // 2) Initialiser les systèmes
    inputSystem_.init(configService_.getEncoderConfigs(), configService_.getButtonConfigs());
    midiSystem_.init(navigationConfig_);
    
    // 3) Initialiser le service d'interface utilisateur
    uiService_.init(navigationConfig_);
    uiService_.setupDebugSubscriptions();
}

void MidiControllerApp::tick()
{
    // 1) Mise à jour des entrées
    inputSystem_.update();

    // 2) Traitement MIDI
    midiSystem_.update();
}
