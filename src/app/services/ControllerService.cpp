#include "app/services/ControllerService.hpp"

#include "app/di/DependencyContainer.hpp"
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

ControllerService::ControllerService(std::shared_ptr<DependencyContainer> container)
    : container_(container),
      viewManager_(*container->resolve<ViewManager>()),
      midiOut_(*container->resolve<IMidiOut>()),
      profileManager_(*container->resolve<IProfileManager>()),
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
        
        if (uiController) {
            uiController_ = uiController.get();
        } else {
            // UIController nécessite un MenuController, donc nous devons le créer
            // ou le récupérer du container d'abord
            auto menuController = container_->resolve<MenuController>();
            if (!menuController) {
                // Créer un MenuController si non disponible
                menuController = std::make_shared<MenuController>(viewManager_, commandManager_);
                container_->registerDependency<MenuController>(menuController);
            }
            
            // Créer un UIController avec ViewManager et MenuController
            auto newUIController = std::make_shared<UIController>(viewManager_, *menuController);
            container_->registerDependency<UIController>(newUIController);
            uiController_ = newUIController.get();
        }
        
        if (inputController) {
            inputController_ = inputController.get();
        } else {
            // Créer un InputController par défaut si aucun n'est trouvé
            auto navConfig = container_->resolve<NavigationConfigService>();
            if (navConfig) {
                auto newInputController = std::make_shared<InputController>(navConfig);
                container_->registerDependency<InputController>(newInputController);
                inputController_ = newInputController.get();
            } else {
                // Création d'un service de navigation par défaut
                auto newNavConfig = std::make_shared<NavigationConfigService>();
                container_->registerDependency<NavigationConfigService>(newNavConfig);
                
                auto newInputController = std::make_shared<InputController>(newNavConfig);
                container_->registerDependency<InputController>(newInputController);
                inputController_ = newInputController.get();
            }
        }
    } else {
        // Cas non supporté - nous avons besoin d'un conteneur
        #ifdef DEBUG
        Serial.println(F("ERREUR: ControllerService nécessite un conteneur de dépendances"));
        #endif
        return;
    }
    
    // Configurer les interactions entre contrôleurs
    if (inputController_ && uiController_) {
        std::shared_ptr<UIController> uiControllerSp = container_->resolve<UIController>();
        if (uiControllerSp) {
            auto inputControllerSp = container_->resolve<InputController>();
            if (inputControllerSp) {
                inputControllerSp->setUIController(uiControllerSp);
            }
        }
    }
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
        
        // Situation critique - nous avons besoin d'un UIController
        #ifdef DEBUG
        Serial.println(F("ERREUR CRITIQUE: UIController non disponible"));
        #endif
        
        // Retourner un objet null en cas d'échec (pour éviter les crashs)
        static UIController* nullController = nullptr;
        return *nullController;
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
        
        // Situation critique - nous avons besoin d'un InputController
        #ifdef DEBUG
        Serial.println(F("ERREUR CRITIQUE: InputController non disponible"));
        #endif
        
        // Retourner un objet null en cas d'échec (pour éviter les crashs)
        static InputController* nullController = nullptr;
        return *nullController;
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