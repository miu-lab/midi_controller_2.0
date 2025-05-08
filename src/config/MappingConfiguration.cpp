#include "config/MappingConfiguration.hpp"

#include <algorithm>

MappingConfiguration::MappingConfiguration() {
    initializeMidiMappings();
    initializeNavigationControls();
    buildMappingIndex();
}

const MidiControl* MappingConfiguration::getMidiMappingForControl(ControlId controlId) const {
    auto it = midiMappingIndex.find(controlId);
    if (it != midiMappingIndex.end()) {
        return &midiMappings[it->second].midiControl;
    }
    return nullptr;
}

bool MappingConfiguration::isNavigationControl(ControlId controlId) const {
    return std::find_if(navigationControls.begin(), navigationControls.end(),
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
                         .relative = false}},  // Encodeur 1 -> CC 1, Canal 1
        {.controlId = 72,
         .midiControl = {.channel = 0,
                         .control = 2,
                         .relative = false}},  // Encodeur 2 -> CC 2, Canal 1
        {.controlId = 73,
         .midiControl = {.channel = 0,
                         .control = 3,
                         .relative = false}},  // Encodeur 3 -> CC 3, Canal 1
        {.controlId = 74,
         .midiControl = {.channel = 0,
                         .control = 4,
                         .relative = false}},  // Encodeur 4 -> CC 4, Canal 1
        {.controlId = 75,
         .midiControl = {.channel = 0,
                         .control = 5,
                         .relative = false}},  // Encodeur 5 -> CC 5, Canal 1
        {.controlId = 76,
         .midiControl = {.channel = 0,
                         .control = 6,
                         .relative = false}},  // Encodeur 6 -> CC 6, Canal 1
        {.controlId = 77,
         .midiControl = {.channel = 0,
                         .control = 7,
                         .relative = false}},  // Encodeur 7 -> CC 7, Canal 1
        {.controlId = 78,
         .midiControl = {.channel = 0,
                         .control = 8,
                         .relative = false}},  // Encodeur 8 -> CC 8, Canal 1
        {.controlId = 80,
         .midiControl = {.channel = 0,
                         .control = 10,
                         .relative = false}},  // Encodeur optique -> CC 10, Canal 1

        // Boutons d'encodeurs (Notes)
        {.controlId = 71,
         .midiControl = {.channel = 0,
                         .control = 36,
                         .relative = false}},  // Bouton Encodeur 1 -> Note 36, Canal 1
        {.controlId = 72,
         .midiControl = {.channel = 0,
                         .control = 37,
                         .relative = false}},  // Bouton Encodeur 2 -> Note 37, Canal 1
        {.controlId = 73,
         .midiControl = {.channel = 0,
                         .control = 38,
                         .relative = false}},  // Bouton Encodeur 3 -> Note 38, Canal 1
        {.controlId = 74,
         .midiControl = {.channel = 0,
                         .control = 39,
                         .relative = false}},  // Bouton Encodeur 4 -> Note 39, Canal 1
        {.controlId = 75,
         .midiControl = {.channel = 0,
                         .control = 40,
                         .relative = false}},  // Bouton Encodeur 5 -> Note 40, Canal 1
        {.controlId = 76,
         .midiControl = {.channel = 0,
                         .control = 41,
                         .relative = false}},  // Bouton Encodeur 6 -> Note 41, Canal 1
        {.controlId = 77,
         .midiControl = {.channel = 0,
                         .control = 42,
                         .relative = false}},  // Bouton Encodeur 7 -> Note 42, Canal 1
        {.controlId = 78,
         .midiControl = {.channel = 0,
                         .control = 43,
                         .relative = false}},  // Bouton Encodeur 8 -> Note 43, Canal 1
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
    for (size_t i = 0; i < midiMappings.size(); ++i) {
        midiMappingIndex[midiMappings[i].controlId] = i;
    }
}
