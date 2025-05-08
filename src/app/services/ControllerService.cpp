#include "app/services/ControllerService.hpp"

#include "app/services/ServiceLocator.hpp"
#include "core/domain/events/EventTypes.hpp"

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
    auto& encoderEventBus = ServiceLocator::getEncoderTurnedEventBus();
    auto& encoderButtonEventBus = ServiceLocator::getEncoderButtonEventBus();
    auto& buttonPressedEventBus = ServiceLocator::getButtonPressedEventBus();
    auto& buttonReleasedEventBus = ServiceLocator::getButtonReleasedEventBus();

    // S'abonner aux événements d'encodeur
    encoderEventBus.subscribe([this](const EventTypes::EncoderTurned& event) {
        // TODO: Déterminer si c'est un encodeur de navigation ou MIDI
        // Pour l'instant, on suppose que l'encodeur 79 est pour la navigation
        if (event.id == 79) {
            // Encodeur de navigation
            if (menuController_.isInMenu()) {
                // Si on est dans un menu, naviguer
                if (event.absolutePosition > 0) {
                    menuController_.selectNextItem();
                } else {
                    menuController_.selectPreviousItem();
                }
            }
        } else {
            // Encodeur MIDI
            midiMapper_.processEncoderChange(event.id, event.absolutePosition);
        }
    });

    // S'abonner aux événements de bouton d'encodeur
    encoderButtonEventBus.subscribe([this](const EventTypes::EncoderButton& event) {
        // TODO: Déterminer si c'est un bouton de navigation ou MIDI
        // Pour l'instant, on suppose que le bouton d'encodeur 79 est pour la navigation
        if (event.id == 79) {
            // Bouton de navigation
            if (event.pressed) {
                if (menuController_.isInMenu()) {
                    // Si on est dans un menu, sélectionner l'élément
                    // TODO: Action spécifique selon l'élément sélectionné
                } else {
                    // Sinon, entrer dans le menu
                    menuController_.enterMenu();
                }
            }
        } else {
            // Bouton MIDI
            midiMapper_.processEncoderButton(event.id, event.pressed);
        }
    });

    // S'abonner aux événements de bouton pressé
    buttonPressedEventBus.subscribe([this](const EventTypes::ButtonPressed& event) {
        // TODO: Déterminer si c'est un bouton de navigation ou MIDI
        // Pour l'instant, on suppose que les boutons 51 et 52 sont pour la navigation
        if (event.id == 51) {
            // Bouton Menu
            menuController_.enterMenu();
        } else if (event.id == 52) {
            // Bouton Valider
            // Action selon le contexte
        } else {
            // Bouton MIDI
            midiMapper_.processButtonPress(event.id, true);
        }
    });

    // S'abonner aux événements de bouton relâché
    buttonReleasedEventBus.subscribe([this](const EventTypes::ButtonReleased& event) {
        // Pour les boutons MIDI uniquement
        if (event.id != 51 && event.id != 52) {
            midiMapper_.processButtonPress(event.id, false);
        }
    });
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
