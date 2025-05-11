#include "config/MappingConfiguration.hpp"

#include <algorithm>
#include <Arduino.h>

MappingConfiguration::MappingConfiguration() {
    initializeMidiMappings();
    initializeNavigationControls();
    buildMappingIndex();
}

const MidiControl* MappingConfiguration::getMidiMappingForControl(ControlId controlId, ControlType controlType) const {
    uint32_t compositeKey = (static_cast<uint32_t>(controlId) << 8) | static_cast<uint8_t>(controlType);
    auto it = midiMappingIndex.find(compositeKey);
    if (it != midiMappingIndex.end()) {
        return &midiMappings[it->second].midiControl;
    }
    return nullptr;
}

bool MappingConfiguration::isNavigationControl(ControlId controlId) const {
    return std::find_if(navigationControls.begin(),
                        navigationControls.end(),
                        [controlId](const NavigationControl& control) {
                            return control.id == controlId;
                        }) != navigationControls.end();
}

const std::vector<MidiControlMapping>& MappingConfiguration::getAllMidiMappings() const {
    return midiMappings;
}

const std::vector<NavigationControl>& MappingConfiguration::getNavigationControls() const {
    return navigationControls;
}

void MappingConfiguration::initializeMidiMappings() {
    // Reprise des configurations actuelles avec une meilleure organisation
    midiMappings = {
        // Encodeurs rotatifs (CCs)
        {.controlId = 71,
         .midiControl = {.channel = 0,
                         .control = 1,
                         .relative = false,
                         .controlType = ControlType::ENCODER_ROTATION}},  // Encodeur 1 -> CC 1, Canal 1

        {.controlId = 72,
         .midiControl = {.channel = 0,
                         .control = 2,
                         .relative = false,
                         .controlType = ControlType::ENCODER_ROTATION}},  // Encodeur 2 -> CC 2, Canal 1

        {.controlId = 73,
         .midiControl = {.channel = 0,
                         .control = 3,
                         .relative = false,
                         .controlType = ControlType::ENCODER_ROTATION}},  // Encodeur 3 -> CC 3, Canal 1

        {.controlId = 74,
         .midiControl = {.channel = 0,
                         .control = 4,
                         .relative = false,
                         .controlType = ControlType::ENCODER_ROTATION}},  // Encodeur 4 -> CC 4, Canal 1

        {.controlId = 75,
         .midiControl = {.channel = 0,
                         .control = 5,
                         .relative = false,
                         .controlType = ControlType::ENCODER_ROTATION}},  // Encodeur 5 -> CC 5, Canal 1

        {.controlId = 76,
         .midiControl = {.channel = 0,
                         .control = 6,
                         .relative = false,
                         .controlType = ControlType::ENCODER_ROTATION}},  // Encodeur 6 -> CC 6, Canal 1

        {.controlId = 77,
         .midiControl = {.channel = 0,
                         .control = 7,
                         .relative = false,
                         .controlType = ControlType::ENCODER_ROTATION}},  // Encodeur 7 -> CC 7, Canal 1

        {.controlId = 78,
         .midiControl = {.channel = 0,
                         .control = 8,
                         .relative = false,
                         .controlType = ControlType::ENCODER_ROTATION}},  // Encodeur 8 -> CC 8, Canal 1

        {.controlId = 80,
         .midiControl = {.channel = 0,
                         .control = 10,
                         .relative = false,
                         .controlType = ControlType::ENCODER_ROTATION}},  // Encodeur optique -> CC 10, Canal 1

        // Boutons d'encodeurs (Notes) - Utilisons les mêmes IDs (71-78) que les encodeurs
        // mais avec controlType = ENCODER_BUTTON
        {.controlId = 71,
         .midiControl = {.channel = 0,
                         .control = 36,
                         .relative = false,
                         .controlType = ControlType::ENCODER_BUTTON}},  // Bouton Encodeur 1 -> Note 36, Canal 1

        {.controlId = 72,
         .midiControl = {.channel = 0,
                         .control = 37,
                         .relative = false,
                         .controlType = ControlType::ENCODER_BUTTON}},  // Bouton Encodeur 2 -> Note 37, Canal 1

        {.controlId = 73,
         .midiControl = {.channel = 0,
                         .control = 38,
                         .relative = false,
                         .controlType = ControlType::ENCODER_BUTTON}},  // Bouton Encodeur 3 -> Note 38, Canal 1

        {.controlId = 74,
         .midiControl = {.channel = 0,
                         .control = 39,
                         .relative = false,
                         .controlType = ControlType::ENCODER_BUTTON}},  // Bouton Encodeur 4 -> Note 39, Canal 1

        {.controlId = 75,
         .midiControl = {.channel = 0,
                         .control = 40,
                         .relative = false,
                         .controlType = ControlType::ENCODER_BUTTON}},  // Bouton Encodeur 5 -> Note 40, Canal 1

        {.controlId = 76,
         .midiControl = {.channel = 0,
                         .control = 41,
                         .relative = false,
                         .controlType = ControlType::ENCODER_BUTTON}},  // Bouton Encodeur 6 -> Note 41, Canal 1

        {.controlId = 77,
         .midiControl = {.channel = 0,
                         .control = 42,
                         .relative = false,
                         .controlType = ControlType::ENCODER_BUTTON}},  // Bouton Encodeur 7 -> Note 42, Canal 1

        {.controlId = 78,
         .midiControl = {.channel = 0,
                         .control = 43,
                         .relative = false,
                         .controlType = ControlType::ENCODER_BUTTON}},  // Bouton Encodeur 8 -> Note 43, Canal 1
        
        // Boutons indépendants
        {.controlId = 51,
         .midiControl = {.channel = 0,
                         .control = 44,
                         .relative = false,
                         .controlType = ControlType::BUTTON}},  // Bouton Menu -> Note 44, Canal 1
        
        {.controlId = 52,
         .midiControl = {.channel = 0,
                         .control = 45,
                         .relative = false, 
                         .controlType = ControlType::BUTTON}},  // Bouton Validation -> Note 45, Canal 1
        
        // Encodeur de navigation (pour quand il n'est pas utilisé pour la navigation)
        {.controlId = 79,
         .midiControl = {.channel = 0,
                         .control = 9,
                         .relative = false,
                         .controlType = ControlType::ENCODER_ROTATION}},  // Encodeur navigation -> CC 9, Canal 1
         
        {.controlId = 79,
         .midiControl = {.channel = 0,
                         .control = 46,
                         .relative = false,
                         .controlType = ControlType::ENCODER_BUTTON}},  // Bouton encodeur navigation -> Note 46, Canal 1
    };
    
}

void MappingConfiguration::initializeNavigationControls() {
    // Reprise des configurations actuelles avec des commentaires plus clairs
    navigationControls = {
        {79},  // Encodeur de navigation principal
        {51},  // Bouton Menu
        {52},  // Bouton de validation
    };
    
}

void MappingConfiguration::buildMappingIndex() {
    // Construit un index pour un accès rapide aux mappings MIDI par ID de contrôle
    midiMappingIndex.clear(); // S'assurer que l'index est vide
    
    for (size_t i = 0; i < midiMappings.size(); ++i) {
        ControlId controlId = midiMappings[i].controlId;
        ControlType controlType = midiMappings[i].midiControl.controlType;
        
        // Utilisation d'une clé composite pour distinguer les contrôles du même ID
        // mais de types différents (encoder vs button)
        uint32_t compositeKey = (static_cast<uint32_t>(controlId) << 8) | static_cast<uint8_t>(controlType);
        midiMappingIndex[compositeKey] = i;
        
    }
    
}