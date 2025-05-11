#include "app/services/ControllerService.hpp"

#include "app/di/DependencyContainer.hpp"
// ServiceLocator est maintenant déprécié, utiliser ServiceLocatorAdapter à la place
#include "app/di/ServiceLocatorAdapter.hpp"
#include "core/domain/events/EventSystem.hpp"
#include "app/services/ControllerServiceListener.hpp"

ControllerService::ControllerService(ViewManager& viewManager, IMidiOut& midiOut,
                                     IProfileManager& profileManager)
    : container_(nullptr),
      viewManager_(viewManager),
      midiOut_(midiOut),
      profileManager_(profileManager),
      usingContainer_(false),
      commandManager_(),
      menuController_(viewManager_, commandManager_),
      midiMapper_(midiOut_, commandManager_),
      profileController_(profileManager_),
      uiController_(nullptr),
      inputController_(nullptr) {
    // Appeler explicitement initializeDependencies() lors de init()
}

// Utiliser des méthodes statiques de l'adaptateur pour initialiser les références
ControllerService::ControllerService(std::shared_ptr<DependencyContainer> container)
    : container_(container),
      // Utiliser les méthodes d'interface spécifiques
      viewManager_(ServiceLocatorAdapter::getViewManagerStatic()),
      midiOut_(ServiceLocatorAdapter::getMidiOutStatic()),
      profileManager_(ServiceLocatorAdapter::getProfileManagerInterfaceStatic()),
      usingContainer_(true),
      commandManager_(),
      menuController_(viewManager_, commandManager_),
      midiMapper_(midiOut_, commandManager_),
      profileController_(profileManager_),
      uiController_(nullptr),
      inputController_(nullptr) {
    // Appeler explicitement initializeDependencies() lors de init()
}

void ControllerService::init() {
    // Initialiser les dépendances (UIController et InputController)
    initializeDependencies();
    
    // Initialiser les mappings MIDI
    initializeMidiMappings();

    // S'abonner aux événements d'entrée pour le MIDI
    auto& eventBus = EventBus::getInstance();
    
    // S'abonner au bus d'événements
    eventListener_ = std::make_shared<ControllerServiceEventListener>(*this);
    eventBus.subscribe(eventListener_.get());
}

void ControllerService::initializeDependencies() {
    if (usingContainer_ && container_) {
        // Utiliser le container pour récupérer les contrôleurs
        auto uiController = container_->resolve<UIController>();
        auto inputController = container_->resolve<InputController>();
        
        // Si les contrôleurs ne sont pas trouvés dans le container, fallback vers ServiceLocatorAdapter
        if (uiController) {
            uiController_ = uiController.get();
        } else {
            uiController_ = &ServiceLocatorAdapter::getUIControllerStatic();
        }
        
        if (inputController) {
            inputController_ = inputController.get();
        } else {
            inputController_ = &ServiceLocatorAdapter::getInputControllerStatic();
        }
    } else {
        // Toujours utiliser ServiceLocatorAdapter
        uiController_ = &ServiceLocatorAdapter::getUIControllerStatic();
        inputController_ = &ServiceLocatorAdapter::getInputControllerStatic();
    }
    
    // Configurer les interactions entre contrôleurs
    std::shared_ptr<UIController> uiControllerSp(uiController_, [](UIController*){});
    inputController_->setUIController(uiControllerSp);
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
        // Essayer le container d'abord si disponible
        if (usingContainer_ && container_) {
            auto uiController = container_->resolve<UIController>();
            if (uiController) {
                uiController_ = uiController.get();
                return *uiController_;
            }
        }
        // Utiliser ServiceLocatorAdapter
        uiController_ = &ServiceLocatorAdapter::getUIControllerStatic();
    }
    return *uiController_;
}

InputController& ControllerService::getInputController() {
    if (!inputController_) {
        // Essayer le container d'abord si disponible
        if (usingContainer_ && container_) {
            auto inputController = container_->resolve<InputController>();
            if (inputController) {
                inputController_ = inputController.get();
                return *inputController_;
            }
        }
        // Utiliser ServiceLocatorAdapter
        inputController_ = &ServiceLocatorAdapter::getInputControllerStatic();
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
