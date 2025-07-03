#pragma once

#include "config/unified/ControlDefinition.hpp"
#include <vector>

/**
 * @brief Données de test pour les configurations
 * 
 * Fournit des configurations prédéfinies pour les tests
 * sans dépendre des vraies configurations du système.
 */
namespace TestConfigurations {

    // Configuration minimale pour tests de base
    inline std::vector<ControlDefinition> getMinimalConfiguration() {
        std::vector<ControlDefinition> controls;
        
        // Encodeur simple
        ControlDefinition encoder;
        encoder.id = 71;
        encoder.name = "test_encoder_1";
        encoder.label = "Test Encoder 1";
        encoder.group = "test";
        encoder.enabled = true;
        encoder.hardware.type = InputType::ENCODER;
        
        ControlDefinition::EncoderConfig encoderConfig;
        encoderConfig.pinA = {2};
        encoderConfig.pinB = {3};
        encoderConfig.ppr = 20;
        encoderConfig.sensitivity = 1.0f;
        encoderConfig.enableAcceleration = false;
        encoderConfig.stepsPerDetent = 4;
        encoder.hardware.config = encoderConfig;
        
        // Mapping MIDI
        ControlDefinition::MappingSpec midiMapping;
        midiMapping.role = MappingRole::MIDI;
        midiMapping.appliesTo = MappingControlType::ENCODER;
        ControlDefinition::MidiConfig midiConfig;
        midiConfig.control = 1;  // CC1
        midiConfig.channel = 0;  // Channel 1
        midiConfig.isRelative = false;
        midiMapping.config = midiConfig;
        encoder.mappings.push_back(midiMapping);
        
        controls.push_back(encoder);
        
        // Bouton simple
        ControlDefinition button;
        button.id = 91;
        button.name = "test_button_1";
        button.label = "Test Button 1";
        button.group = "test";
        button.enabled = true;
        button.hardware.type = InputType::BUTTON;
        
        ControlDefinition::ButtonConfig buttonConfig;
        buttonConfig.pin = {10};
        buttonConfig.activeLow = true;
        buttonConfig.mode = ButtonMode::MOMENTARY;
        buttonConfig.debounceMs = 50;
        button.hardware.config = buttonConfig;
        
        controls.push_back(button);
        
        return controls;
    }

    // Configuration complexe pour tests avancés
    inline std::vector<ControlDefinition> getComplexConfiguration() {
        std::vector<ControlDefinition> controls;
        
        // 4 encodeurs avec boutons intégrés
        for (int i = 0; i < 4; i++) {
            ControlDefinition encoder;
            encoder.id = 71 + i;
            encoder.name = "enc_" + std::to_string(i + 1);
            encoder.label = "ENC" + std::to_string(i + 1);
            encoder.group = "encoders";
            encoder.enabled = true;
            encoder.hardware.type = InputType::ENCODER;
            
            ControlDefinition::EncoderConfig encoderConfig;
            encoderConfig.pinA = {static_cast<uint8_t>(2 + (i * 2))};
            encoderConfig.pinB = {static_cast<uint8_t>(3 + (i * 2))};
            encoderConfig.ppr = 20;
            encoderConfig.sensitivity = 1.0f;
            encoderConfig.enableAcceleration = false;
            encoderConfig.stepsPerDetent = 4;
            encoder.hardware.config = encoderConfig;
            encoder.hardware.encoderButtonPin = GpioPin{static_cast<uint8_t>(20 + i)};  // Bouton intégré
            encoder.hardware.encoderButtonDebounceMs = 50;
            
            // Mapping MIDI
            ControlDefinition::MappingSpec midiMapping;
            midiMapping.role = MappingRole::MIDI;
            midiMapping.appliesTo = MappingControlType::ENCODER;
            ControlDefinition::MidiConfig midiConfig;
            midiConfig.control = i + 1;  // CC1-4
            midiConfig.channel = 0;      // Channel 1
            midiConfig.isRelative = false;
            midiMapping.config = midiConfig;
            encoder.mappings.push_back(midiMapping);
            
            controls.push_back(encoder);
        }
        
        // 4 boutons indépendants
        for (int i = 0; i < 4; i++) {
            ControlDefinition button;
            button.id = 91 + i;
            button.name = "btn_" + std::to_string(i + 1);
            button.label = "BTN" + std::to_string(i + 1);
            button.group = "buttons";
            button.enabled = true;
            button.hardware.type = InputType::BUTTON;
            
            ControlDefinition::ButtonConfig buttonConfig;
            buttonConfig.pin = {static_cast<uint8_t>(30 + i)};
            buttonConfig.activeLow = true;
            buttonConfig.mode = ButtonMode::MOMENTARY;
            buttonConfig.debounceMs = 50;
            button.hardware.config = buttonConfig;
            
            controls.push_back(button);
        }
        
        return controls;
    }

    // Configuration avec boutons enfants d'encodeurs
    inline std::vector<ControlDefinition> getEncoderWithChildButtonsConfiguration() {
        std::vector<ControlDefinition> controls;
        
        // Encodeur parent
        ControlDefinition encoder;
        encoder.id = 71;
        encoder.name = "master_encoder";
        encoder.label = "Master Encoder";
        encoder.group = "master";
        encoder.enabled = true;
        encoder.hardware.type = InputType::ENCODER;
        
        ControlDefinition::EncoderConfig encoderConfig;
        encoderConfig.pinA = {2};
        encoderConfig.pinB = {3};
        encoderConfig.ppr = 20;
        encoderConfig.sensitivity = 1.0f;
        encoderConfig.enableAcceleration = false;
        encoderConfig.stepsPerDetent = 4;
        encoder.hardware.config = encoderConfig;
        
        // Mapping MIDI
        ControlDefinition::MappingSpec midiMapping;
        midiMapping.role = MappingRole::MIDI;
        midiMapping.appliesTo = MappingControlType::ENCODER;
        ControlDefinition::MidiConfig midiConfig;
        midiConfig.control = 1;
        midiConfig.channel = 0;
        midiConfig.isRelative = false;
        midiMapping.config = midiConfig;
        encoder.mappings.push_back(midiMapping);
        
        controls.push_back(encoder);
        
        // Bouton enfant
        ControlDefinition childButton;
        childButton.id = 91;
        childButton.name = "master_button";
        childButton.label = "Master Button";
        childButton.group = "master";
        childButton.enabled = true;
        childButton.hardware.type = InputType::BUTTON;
        
        ControlDefinition::ButtonConfig buttonConfig;
        buttonConfig.pin = {10};
        buttonConfig.activeLow = true;
        buttonConfig.mode = ButtonMode::MOMENTARY;
        buttonConfig.debounceMs = 50;
        childButton.hardware.config = buttonConfig;
        childButton.parentId = 71;  // Parent = encodeur
        
        controls.push_back(childButton);
        
        return controls;
    }

    // Configuration vide pour tests edge cases
    inline std::vector<ControlDefinition> getEmptyConfiguration() {
        return std::vector<ControlDefinition>();
    }

    // Configuration avec erreurs pour tests de validation
    inline std::vector<ControlDefinition> getInvalidConfiguration() {
        std::vector<ControlDefinition> controls;
        
        // Encodeur avec pins invalides
        ControlDefinition invalidEncoder;
        invalidEncoder.id = 0;  // ID invalide
        invalidEncoder.name = "";  // Name vide
        invalidEncoder.label = "";  // Label vide
        invalidEncoder.group = "invalid";
        invalidEncoder.enabled = true;
        invalidEncoder.hardware.type = InputType::ENCODER;
        
        ControlDefinition::EncoderConfig invalidConfig;
        invalidConfig.pinA = {255};  // Pin invalide
        invalidConfig.pinB = {255};  // Pin invalide
        invalidConfig.ppr = 0;     // PPR invalide
        invalidConfig.sensitivity = -1.0f;  // Sensibilité invalide
        invalidConfig.enableAcceleration = false;
        invalidConfig.stepsPerDetent = 0;  // Steps invalide
        invalidEncoder.hardware.config = invalidConfig;
        
        controls.push_back(invalidEncoder);
        
        return controls;
    }
}