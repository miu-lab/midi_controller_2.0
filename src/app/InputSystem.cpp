#include "app/InputSystem.hpp"

InputSystem::InputSystem()
    : encoderManager_({})
    , processEncoders_(encoderManager_.getEncoders())
    , buttonManager_({})
    , processButtons_(buttonManager_.getButtons())
{
}

void InputSystem::init() {
    // Initialiser l'état initial des encodeurs et boutons
    encoderManager_.updateAll();
    buttonManager_.updateAll();
    processButtons_.initStates(); // Initialiser les états sans déclencher d'événements
}

void InputSystem::init(const std::vector<EncoderConfig>& encoderConfigs, 
                       const std::vector<ButtonConfig>& buttonConfigs) {
    // Reconfigurer les gestionnaires
    encoderManager_ = EncoderManager(encoderConfigs);
    processEncoders_ = ProcessEncoders(encoderManager_.getEncoders());
    buttonManager_ = DigitalButtonManager(buttonConfigs);
    processButtons_ = ProcessButtons(buttonManager_.getButtons());
    
    // Initialiser l'état initial
    init();
}

void InputSystem::update() {
    // Lecture et publication des mouvements d'encodeurs
    encoderManager_.updateAll();
    processEncoders_.update();

    // Lecture et publication des états de boutons
    buttonManager_.updateAll();
    processButtons_.update();
}
