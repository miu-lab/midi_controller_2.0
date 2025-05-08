// app/ControllerApp.cpp
#include "app/ControllerApp.hpp"
#include <Arduino.h>
#include "utils/Scheduler.hpp"
#include "utils/EventBus.hpp"
#include "input/InputEvent.hpp"

ControllerApp::ControllerApp()
    // Gestion des profils et navigation
    : profileManager_()
    // Systèmes
    , inputSystem_()
    , midiSystem_(profileManager_)
{
}

void ControllerApp::setControlForNavigation(ControlId id, bool isNavigation) {
    navigationConfig_.setControlForNavigation(id, isNavigation);
}

bool ControllerApp::isNavigationControl(ControlId id) const {
    return navigationConfig_.isNavigationControl(id);
}

void ControllerApp::begin()
{
    // Ne pas attendre le port série pour ne pas bloquer le MIDI
    // Optionnel: ajouter un timeout pour permettre une connexion série si disponible rapidement
    // unsigned long startTime = millis();
    // while (!Serial && (millis() - startTime < 1000)) { /* attend jusqu'à 1 seconde */ }

    // 1) Charger toutes les configurations
    configService_.loadDefaultConfigurations();
    
    // 2) Appliquer les configurations aux différents services
    configService_.applyConfigurations(profileManager_, navigationConfig_);
    
    // 3) Initialiser les systèmes
    inputSystem_.init(configService_.getEncoderConfigs(), configService_.getButtonConfigs());
    midiSystem_.init(navigationConfig_);
    
    // 4) Initialiser le service d'interface utilisateur
    uiService_.init(navigationConfig_);
    uiService_.setupDebugSubscriptions();
}

void ControllerApp::tick()
{
    // 1) Mise à jour des entrées
    inputSystem_.update();

    // 2) Traitement MIDI
    midiSystem_.update();
}
