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
        encoder.label = "Test Encoder 1";
        encoder.group = "test";
        encoder.enabled = true;
        encoder.hardware.type = InputType::ENCODER;
        encoder.hardware.encoderPinA = 2;
        encoder.hardware.encoderPinB = 3;
        
        // Mapping MIDI
        ControlDefinition::MidiMapping midiMapping;
        midiMapping.role = MappingRole::MIDI;
        ControlDefinition::MidiConfig midiConfig;
        midiConfig.control = 1;  // CC1
        midiConfig.channel = 0;  // Channel 1
        midiMapping.config = midiConfig;
        encoder.mappings.push_back(midiMapping);
        
        controls.push_back(encoder);
        
        // Bouton simple
        ControlDefinition button;
        button.id = 91;
        button.label = "Test Button 1";
        button.group = "test";
        button.enabled = true;
        button.hardware.type = InputType::BUTTON;
        button.hardware.buttonPin = 10;
        
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
            encoder.label = "ENC" + std::to_string(i + 1);
            encoder.group = "encoders";
            encoder.enabled = true;
            encoder.hardware.type = InputType::ENCODER;
            encoder.hardware.encoderPinA = 2 + (i * 2);
            encoder.hardware.encoderPinB = 3 + (i * 2);
            encoder.hardware.encoderButtonPin = 20 + i;  // Bouton intégré
            
            // Mapping MIDI
            ControlDefinition::MidiMapping midiMapping;
            midiMapping.role = MappingRole::MIDI;
            ControlDefinition::MidiConfig midiConfig;
            midiConfig.control = i + 1;  // CC1-4
            midiConfig.channel = 0;      // Channel 1
            midiMapping.config = midiConfig;
            encoder.mappings.push_back(midiMapping);
            
            controls.push_back(encoder);
        }
        
        // 4 boutons indépendants
        for (int i = 0; i < 4; i++) {
            ControlDefinition button;
            button.id = 91 + i;
            button.label = "BTN" + std::to_string(i + 1);
            button.group = "buttons";
            button.enabled = true;
            button.hardware.type = InputType::BUTTON;
            button.hardware.buttonPin = 30 + i;
            
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
        encoder.label = "Master Encoder";
        encoder.group = "master";
        encoder.enabled = true;
        encoder.hardware.type = InputType::ENCODER;
        encoder.hardware.encoderPinA = 2;
        encoder.hardware.encoderPinB = 3;
        
        // Mapping MIDI
        ControlDefinition::MidiMapping midiMapping;
        midiMapping.role = MappingRole::MIDI;
        ControlDefinition::MidiConfig midiConfig;
        midiConfig.control = 1;
        midiConfig.channel = 0;
        midiMapping.config = midiConfig;
        encoder.mappings.push_back(midiMapping);
        
        controls.push_back(encoder);
        
        // Bouton enfant
        ControlDefinition childButton;
        childButton.id = 91;
        childButton.label = "Master Button";
        childButton.group = "master";
        childButton.enabled = true;
        childButton.hardware.type = InputType::BUTTON;
        childButton.hardware.buttonPin = 10;
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
        invalidEncoder.label = "";  // Label vide
        invalidEncoder.group = "invalid";
        invalidEncoder.enabled = true;
        invalidEncoder.hardware.type = InputType::ENCODER;
        invalidEncoder.hardware.encoderPinA = 255;  // Pin invalide
        invalidEncoder.hardware.encoderPinB = 255;  // Pin invalide
        
        controls.push_back(invalidEncoder);
        
        return controls;
    }
}