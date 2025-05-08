// app/ControllerApp.cpp
#include "app/ControllerApp.hpp"
#include <Arduino.h>
#include "utils/Scheduler.hpp"
#include "utils/EventBus.hpp"
#include "input/InputEvent.hpp" // pour EncoderMoved, ButtonPressed, ButtonReleased

// Config des encodeurs
#include "config/ControlEncodersConfig.hpp"
// Config des boutons simples
#include "config/ControlButtonsConfig.hpp"
// Config des mappings MIDI
#include "config/MidiMappingConfig.hpp"

ControllerApp::ControllerApp()
    // Gestionnaires des contrôles physiques
    : encoderManager_(controlEncoderConfigs)
    , processEncoders_(encoderManager_.getEncoders())
    , buttonManager_(controlButtonConfigs)
    , processButtons_(buttonManager_.getButtons())
    // Composants MIDI
    , rawMidiOut_()
    , bufferedMidiOut_(rawMidiOut_)
    , midiInHandler_()
    // Gestion des profils et routage
    , profileManager_()
    , inputRouter_(bufferedMidiOut_, profileManager_)
{
}

void ControllerApp::begin()
{
    // Ne pas attendre le port série pour ne pas bloquer le MIDI
    // Optionnel: ajouter un timeout pour permettre une connexion série si disponible rapidement
    // unsigned long startTime = millis();
    // while (!Serial && (millis() - startTime < 1000)) { /* attend jusqu'à 1 seconde */ }

    // 1) Initialiser le système MIDI
    // Charger les mappings MIDI par défaut depuis la configuration
    for (size_t i = 0; i < defaultMidiMappingCount; i++) {
        const auto& mapping = defaultMidiMappings[i];
        profileManager_.setBinding(mapping.controlId, mapping.midiControl);
    }
    
    // Initialiser le routeur d'entrée (s'abonne aux événements)
    inputRouter_.init();

    // 2) Souscriptions pour debug
    EventBus<EncoderTurnedEvent>::subscribe([](const EncoderTurnedEvent &e)
                                            {
        Serial.print("ENC ");
        Serial.print(e.id);
        Serial.print(" abs:");
        Serial.println(e.absolutePosition);
    });
    EventBus<EncoderButtonEvent>::subscribe([](const EncoderButtonEvent &e)
                                            {
        Serial.print("ENC_BTN ");
        Serial.print(e.id);
        Serial.println(e.pressed ? " PRESSED" : " RELEASED"); });
    EventBus<ButtonPressed>::subscribe([](const ButtonPressed &b)
                                       {
        Serial.print("BTN ");
        Serial.print(b.id);
        Serial.println(" PRESSED"); });
    EventBus<ButtonReleased>::subscribe([](const ButtonReleased &b)
                                        {
        Serial.print("BTN ");
        Serial.print(b.id);
        Serial.println(" RELEASED"); });

    // 3) Initialise l'état initial des encodeurs et boutons
    encoderManager_.updateAll();
    buttonManager_.updateAll();
    processButtons_.initStates(); // Initialiser les états sans déclencher d'événements
}

void ControllerApp::tick()
{
    // 1) Lecture et publication des mouvements d'encodeurs
    encoderManager_.updateAll();
    processEncoders_.update();

    // 2) Lecture et publication des états de boutons
    buttonManager_.updateAll();
    processButtons_.update();

    // 3) Traitement MIDI
    // Lire les messages MIDI entrants
    midiInHandler_.update();
    
    // Envoyer les messages MIDI en attente
    bufferedMidiOut_.flush();
    rawMidiOut_.flush();  // Traiter les messages USB
}
