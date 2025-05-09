#include "app/services/ControllerService.hpp"

#include "app/services/ServiceLocator.hpp"
#include "core/domain/events/EventSystem.hpp"
#include "app/services/ControllerServiceListener.hpp"

ControllerService::ControllerService(ViewManager& viewManager, IMidiOut& midiOut,
                                     IProfileManager& profileManager)
    : viewManager_(viewManager),
      midiOut_(midiOut),
      profileManager_(profileManager),
      commandManager_(),
      menuController_(viewManager_, commandManager_),
      midiMapper_(midiOut_, commandManager_),
      profileController_(profileManager_),
      uiController_(nullptr),
      inputController_(nullptr) {}

void ControllerService::init() {
    // Récupérer les contrôleurs depuis le ServiceLocator
    uiController_ = &ServiceLocator::getUIController();
    inputController_ = &ServiceLocator::getInputController();
    
    // Configurer les interactions entre contrôleurs
    inputController_->setUIController(uiController_);
    
    // Initialiser les mappings MIDI
    initializeMidiMappings();

    // S'abonner aux événements d'entrée pour le MIDI
    auto& eventBus = EventBus::getInstance();
    
    // S'abonner au bus d'événements
    eventListener_ = std::make_unique<ControllerServiceEventListener>(*this);
    eventBus.subscribe(eventListener_.get());
}

void ControllerService::update() {
    // Mettre à jour le contrôleur MIDI pour les commandes temporisées
    midiMapper_.update();
    
    // Mettre à jour le contrôleur UI si nécessaire
    // Cette méthode pourrait être ajoutée dans le futur
}

MenuController& ControllerService::getMenuController() {
    return menuController_;
}

MidiMapper& ControllerService::getMidiMapper() {
    return midiMapper_;
}

ProfileController& ControllerService::getProfileController() {
    return profileController_;
}

UIController& ControllerService::getUIController() {
    if (!uiController_) {
        uiController_ = &ServiceLocator::getUIController();
    }
    return *uiController_;
}

InputController& ControllerService::getInputController() {
    if (!inputController_) {
        inputController_ = &ServiceLocator::getInputController();
    }
    return *inputController_;
}

void ControllerService::initializeMidiMappings() {
    // Récupérer tous les mappings du ProfileManager
    auto mappings = profileController_.getAllMappings();

    // Appliquer chaque mapping au MidiController
    for (const auto& mapping : mappings) {
        // Créer une stratégie appropriée
        auto strategy =
            profileController_.createMappingStrategy(mapping.controlId, mapping.midiControl);

        // Définir le mapping
        midiMapper_.setMapping(mapping.controlId, mapping.midiControl, std::move(strategy));
    }
}
