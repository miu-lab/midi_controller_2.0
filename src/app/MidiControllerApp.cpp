#include "app/MidiControllerApp.hpp"

#include <Arduino.h>

#include "core/TaskScheduler.hpp"
#include "core/controllers/UIController.hpp"
#include "core/domain/events/EventSystem.hpp"
#include "core/listeners/UIControllerEventListener.hpp"

MidiControllerApp::MidiControllerApp(const ApplicationConfiguration& appConfig)
    : configService_(appConfig),
      navigationConfig_(),
      profileManager_(),
      midiSystem_(profileManager_),
      eventInputSystem_(),
      uiEventService_(),
      uiControllerEventListener_(nullptr),
      uiEventListener_(nullptr),
      uiControllerEventListenerSubId_(0),
      uiEventListenerSubId_(0) {
    // Initialisation des services et dépendances
    ServiceLocator::initialize(appConfig);
    ServiceLocator::registerNavigationConfigService(&navigationConfig_);
    ServiceLocator::registerProfileManager(&profileManager_);
    ServiceLocator::registerMidiSystem(&midiSystem_);

    InputSystem* inputSystem = &eventInputSystem_;
    ServiceLocator::registerInputSystem(inputSystem);

    ServiceLocator::registerUiEventService(&uiEventService_);
    ServiceLocator::registerConfigurationService(&configService_);
}

MidiControllerApp::~MidiControllerApp() {
    // Désabonnement des écouteurs
    auto& eventBus = EventBus::getInstance();

    if (uiControllerEventListenerSubId_ != 0) {
        eventBus.unsubscribe(uiControllerEventListenerSubId_);
    }

    if (uiEventListenerSubId_ != 0) {
        eventBus.unsubscribe(uiEventListenerSubId_);
    }
}

void MidiControllerApp::setControlForNavigation(ControlId id, bool isNavigation) {
    navigationConfig_.setControlForNavigation(id, isNavigation);
}

bool MidiControllerApp::isNavigationControl(ControlId id) const {
    return navigationConfig_.isNavigationControl(id);
}

void MidiControllerApp::init() {
    // Application des configurations et initialisation des systèmes
    configService_.applyConfigurations(profileManager_, navigationConfig_);
    eventInputSystem_.init(configService_.getEncoderConfigs(), configService_.getButtonConfigs());
    midiSystem_.init(navigationConfig_);
    uiEventService_.init(navigationConfig_);

    // Note: L'UI sera implémentée ultérieurement (ViewManager, MenuController, UIController)
#ifdef DEBUG
    Serial.println(
        F("Note: L'initialisation de UIController est commentée pour l'instant et sera implémentée "
          "dans une étape ultérieure"));
#endif
}

void MidiControllerApp::update() {
    // Mise à jour des systèmes principaux
    eventInputSystem_.update();
    midiSystem_.update();
}