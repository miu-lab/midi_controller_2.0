#include "app/services/EventInputSystem.hpp"
#include "config/debug/DebugConfig.hpp"

EventInputSystem::EventInputSystem()
    : InputSystem(),  // Appeler le constructeur de la classe de base
      encoderManager_({}),
      processEncoders_(encoderManager_.getEncoders()),
      buttonManager_({}),
      processButtons_(buttonManager_.getButtons()) {
}

void EventInputSystem::init() {
    // Initialiser l'état initial des encodeurs et boutons
    encoderManager_.updateAll();
    buttonManager_.updateAll();
    processButtons_.initStates();  // Initialiser les états sans déclencher d'événements
    
    // Configurer les callbacks
    processEncoders_.setOnEncoderTurnedCallback([this](uint8_t id, int32_t position, int8_t delta) {
        this->handleEncoderTurn(id, position, delta);
    });
    
    processEncoders_.setOnEncoderButtonCallback([this](uint8_t id, bool pressed) {
        this->handleEncoderButton(id, pressed);
    });
    
    processButtons_.setOnButtonStateChangedCallback([this](uint8_t id, bool pressed) {
        this->handleButton(id, pressed);
    });
    
#if defined(DEBUG) && defined(DEBUG_INPUT_SYSTEM)
    Serial.println(F("EventInputSystem: Initialized"));
#endif
}

void EventInputSystem::init(const std::vector<EncoderConfig>& encoderConfigs,
                        const std::vector<ButtonConfig>& buttonConfigs) {
    // Reconfigurer les gestionnaires
    encoderManager_ = EncoderManager(encoderConfigs);
    processEncoders_ = ProcessEncoders(encoderManager_.getEncoders());
    buttonManager_ = DigitalButtonManager(buttonConfigs);
    processButtons_ = ProcessButtons(buttonManager_.getButtons());
    
    // Initialiser avec les nouvelles configurations
    init();
}

void EventInputSystem::update() {
    // Lecture et publication des mouvements d'encodeurs
    encoderManager_.updateAll();
    processEncoders_.update();
    
    // Lecture et publication des états de boutons
    buttonManager_.updateAll();
    processButtons_.update();
}

void EventInputSystem::handleEncoderTurn(uint8_t id, int32_t position, int8_t delta) {
    // Créer et publier l'événement sur le bus d'événements
    EncoderTurnedEvent event(id, position, delta);
    EventBus::getInstance().publish(event);
    
#if defined(DEBUG) && defined(DEBUG_INPUT_EVENTS)
    Serial.print(F("Encoder turned: ID="));
    Serial.print(id);
    Serial.print(F(", Position="));
    Serial.print(position);
    Serial.print(F(", Delta="));
    Serial.println(delta);
#endif
}

void EventInputSystem::handleEncoderButton(uint8_t id, bool pressed) {
    // Créer et publier l'événement sur le bus d'événements
    EncoderButtonEvent event(id, pressed);
    EventBus::getInstance().publish(event);
    
#if defined(DEBUG) && defined(DEBUG_INPUT_EVENTS)
    Serial.print(F("Encoder button: ID="));
    Serial.print(id);
    Serial.print(F(", Pressed="));
    Serial.println(pressed ? F("Yes") : F("No"));
#endif
}

void EventInputSystem::handleButton(uint8_t id, bool pressed) {
    // Créer et publier l'événement sur le bus d'événements
    if (pressed) {
        ButtonPressedEvent event(id);
        EventBus::getInstance().publish(event);
    } else {
        ButtonReleasedEvent event(id);
        EventBus::getInstance().publish(event);
    }
    
#if defined(DEBUG) && defined(DEBUG_INPUT_EVENTS)
    Serial.print(F("Button: ID="));
    Serial.print(id);
    Serial.print(F(", Pressed="));
    Serial.println(pressed ? F("Yes") : F("No"));
#endif
}
