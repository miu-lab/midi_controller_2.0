// app/MidiControllerApp.cpp
#include "app/MidiControllerApp.hpp"

#include <Arduino.h>

#include "core/TaskScheduler.hpp"
#include "core/controllers/UIController.hpp"
#include "core/listeners/UIControllerEventListener.hpp"
#include "core/domain/events/EventSystem.hpp"

MidiControllerApp::MidiControllerApp(const ApplicationConfiguration& appConfig)
    // Service de configuration
    : configService_(appConfig)
      // Gestion des profils et navigation
      ,
      navigationConfig_()
      // Systèmes
      ,
      profileManager_()
      ,
      midiSystem_(profileManager_)
      ,
      eventInputSystem_()
      ,
      uiEventService_()
      ,
      uiControllerEventListener_(nullptr),
      uiEventListener_(nullptr),
      uiControllerEventListenerSubId_(0),
      uiEventListenerSubId_(0) {
    // Initialiser le ServiceLocator avec la configuration
    ServiceLocator::initialize(appConfig);

    // Enregistrer les services dans le ServiceLocator
    ServiceLocator::registerNavigationConfigService(&navigationConfig_);
    ServiceLocator::registerProfileManager(&profileManager_);
    ServiceLocator::registerMidiSystem(&midiSystem_);
    
    // Convertir EventInputSystem en InputSystem pour la compatibilité avec ServiceLocator
    InputSystem* inputSystem = &eventInputSystem_;
    ServiceLocator::registerInputSystem(inputSystem);
    
    ServiceLocator::registerUiEventService(&uiEventService_);
    ServiceLocator::registerConfigurationService(&configService_);
}

MidiControllerApp::~MidiControllerApp() {
    // Se désabonner du bus d'événements
    auto& eventBus = EventBus::getInstance();
    
    // Désabonnement et libération des écouteurs
    if (uiControllerEventListenerSubId_ != 0) {
        eventBus.unsubscribe(uiControllerEventListenerSubId_);
    }
    
    if (uiEventListenerSubId_ != 0) {
        eventBus.unsubscribe(uiEventListenerSubId_);
    }
    
    // Libérer les écouteurs
    delete uiControllerEventListener_;
    delete uiEventListener_;
}

void MidiControllerApp::setControlForNavigation(ControlId id, bool isNavigation) {
    navigationConfig_.setControlForNavigation(id, isNavigation);
}

bool MidiControllerApp::isNavigationControl(ControlId id) const {
    return navigationConfig_.isNavigationControl(id);
}

void MidiControllerApp::init() {
    // Ne pas attendre le port série pour ne pas bloquer le MIDI
    // Optionnel: ajouter un timeout pour permettre une connexion série si disponible rapidement
    // unsigned long startTime = millis();
    // while (!Serial && (millis() - startTime < 1000)) { /* attend jusqu'à 1 seconde */ }

    // 1) Appliquer les configurations aux différents services
    configService_.applyConfigurations(profileManager_, navigationConfig_);

    // 2) Initialiser les systèmes
    eventInputSystem_.init(configService_.getEncoderConfigs(), configService_.getButtonConfigs());
    midiSystem_.init(navigationConfig_);

    // 3) Initialiser le service d'interface utilisateur
    uiEventService_.init(navigationConfig_);
    // Note: setupDebugSubscriptions() est déjà appelé dans init(), pas besoin de l'appeler à nouveau
    
    // 4) Initialiser la chaîne UI : ViewManager, MenuController, UIController et UIControllerEventListener
    
    // Note: Pour l'instant, nous ne créons pas réellement ces composants car ils nécessitent
    // des implémentations supplémentaires. Ce commentaire sert de guide pour la mise en œuvre future.
    
    /*
    // Créer le ViewManager
    ViewManager* viewManager = new ViewManager();
    ServiceLocator::registerViewManager(viewManager);
    
    // Créer le MenuController
    MenuController* menuController = new MenuController();
    ServiceLocator::registerMenuController(menuController);
    
    // Créer l'instance du UIController
    UIController* uiController = new UIController(*viewManager, *menuController);
    
    // Enregistrer le UIController dans le ServiceLocator
    ServiceLocator::registerUIController(uiController);
    
    // S'il existe déjà un écouteur, le désabonner et le libérer
    if (uiControllerEventListener_ != nullptr) {
        if (uiControllerEventListenerSubId_ != 0) {
            EventBus::getInstance().unsubscribe(uiControllerEventListenerSubId_);
        }
        delete uiControllerEventListener_;
    }
    
    // Créer et enregistrer l'écouteur d'événements
    uiControllerEventListener_ = new UIControllerEventListener(*uiController, navigationConfig_);
    
    // S'abonner au bus d'événements avec priorité élevée
    uiControllerEventListenerSubId_ = EventBus::getInstance().subscribe(uiControllerEventListener_, 10); // Priorité élevée (10) pour les événements UI
    
    // Enregistrer l'écouteur dans le ServiceLocator
    ServiceLocator::registerUIControllerEventListener(uiControllerEventListener_);
    */
    
#ifdef DEBUG
    Serial.println(F("Note: L'initialisation de UIController est commentée pour l'instant et sera implémentée dans une étape ultérieure"));
#endif
}

void MidiControllerApp::update() {
    // 1) Mise à jour des entrées
    eventInputSystem_.update();

    // 2) Traitement MIDI
    midiSystem_.update();
    
    // 3) Note: La mise à jour des contrôleurs est gérée via les événements
    // Notre écouteur UIControllerEventListener se charge de router les événements
}