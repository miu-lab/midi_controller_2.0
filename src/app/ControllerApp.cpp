// app/ControllerApp.cpp
#include "app/ControllerApp.hpp"
#include <Arduino.h>
#include "utils/Scheduler.hpp"
#include "utils/EventBus.hpp"
#include "input/InputEvent.hpp"          // pour EncoderMoved, ButtonPressed, ButtonReleased

// Config des encodeurs
#include "config/ControlEncodersConfig.hpp"
// Config des boutons simples
#include "config/ControlButtonsConfig.hpp"

ControllerApp::ControllerApp()
    : encoderManager_(controlEncoderConfigs)
    , processEncoders_(encoderManager_.getEncoders())
    , buttonManager_(controlButtonConfigs)
    , processButtons_(buttonManager_.getButtons())
{}

void ControllerApp::begin() {
    while (!Serial) { /* attente du port série pour Teensy */ }

    // 2) Souscriptions pour debug
    EventBus<EncoderTurnedEvent>::subscribe([](const EncoderTurnedEvent& e) {
        Serial.print("ENC ");
        Serial.print(e.id);
        Serial.print(" Δ ");
        Serial.println(e.delta);
    });
    EventBus<ButtonPressed>::subscribe([](const ButtonPressed& b) {
        Serial.print("BTN ");
        Serial.print(b.id);
        Serial.println(" PRESSED");
    });
    EventBus<ButtonReleased>::subscribe([](const ButtonReleased& b) {
        Serial.print("BTN ");
        Serial.print(b.id);
        Serial.println(" RELEASED");
    });

    // 3) Initialise l'état initial des encodeurs et boutons
    encoderManager_.updateAll();
    buttonManager_.updateAll();
}

void ControllerApp::tick() {
    // 1) Lecture et publication des mouvements d'encodeurs
    encoderManager_.updateAll();
    processEncoders_.update();

    // 2) Lecture et publication des états de boutons
    buttonManager_.updateAll();
    processButtons_.update();

    // 3) TODO : Scheduler.update(millis()), rafraîchissement UI, flush MIDI
}
