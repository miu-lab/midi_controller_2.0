#include "config/MappingConfiguration.hpp"

#include <Arduino.h>

#include <algorithm>

MappingConfiguration::MappingConfiguration() {
    initializeMidiMappings();
    initializeNavigationControls();
    buildMappingIndex();
}

const MidiControl* MappingConfiguration::getMidiMappingForControl(InputId controlId,
                                                                  MappingType controlType) const {
    uint32_t compositeKey =
        (static_cast<uint32_t>(controlId) << 8) | static_cast<uint8_t>(controlType);
    auto it = midiMappingIndex.find(compositeKey);
    if (it != midiMappingIndex.end()) {
        return &controlsMapping[it->second].midiMapping;
    }
    return nullptr;
}

bool MappingConfiguration::isNavigationControl(InputId controlId) const {
    return std::find_if(navigationControls.begin(),
                        navigationControls.end(),
                        [controlId](const NavigationControl& control) {
                            return control.id == controlId;
                        }) != navigationControls.end();
}

const std::vector<InputMapping>& MappingConfiguration::getMappedControls() const {
    return controlsMapping;
}

const std::vector<NavigationControl>& MappingConfiguration::getNavigationControls() const {
    return navigationControls;
}

void MappingConfiguration::initializeMidiMappings() {
    // Reprise des configurations actuelles avec une meilleure organisation
    controlsMapping.clear();

    // Encodeurs rotatifs (CCs)
    controlsMapping.emplace_back(
        InputMapping{.controlId = 71,
                     .mappingType = MappingType::ENCODER,
                     .midiMapping = {.channel = 0,
                                     .control = 1,
                                     .isRelative = true}});  // Encodeur 1 -> CC 1, Canal 1

    controlsMapping.emplace_back(
        InputMapping{.controlId = 72,
                     .mappingType = MappingType::ENCODER,
                     .midiMapping = {.channel = 0,
                                     .control = 2,
                                     .isRelative = true}});  // Encodeur 2 -> CC 2, Canal 1

    controlsMapping.emplace_back(
        InputMapping{.controlId = 73,
                     .mappingType = MappingType::ENCODER,
                     .midiMapping = {.channel = 0,
                                     .control = 3,
                                     .isRelative = true}});  // Encodeur 3 -> CC 3, Canal 1

    controlsMapping.emplace_back(
        InputMapping{.controlId = 74,
                     .mappingType = MappingType::ENCODER,
                     .midiMapping = {.channel = 0,
                                     .control = 4,
                                     .isRelative = true}});  // Encodeur 4 -> CC 4, Canal 1

    controlsMapping.emplace_back(
        InputMapping{.controlId = 75,
                     .mappingType = MappingType::ENCODER,
                     .midiMapping = {.channel = 0,
                                     .control = 5,
                                     .isRelative = true}});  // Encodeur 5 -> CC 5, Canal 1

    controlsMapping.emplace_back(
        InputMapping{.controlId = 76,
                     .mappingType = MappingType::ENCODER,
                     .midiMapping = {.channel = 0,
                                     .control = 6,
                                     .isRelative = true}});  // Encodeur 6 -> CC 6, Canal 1

    controlsMapping.emplace_back(
        InputMapping{.controlId = 77,
                     .mappingType = MappingType::ENCODER,
                     .midiMapping = {.channel = 0,
                                     .control = 7,
                                     .isRelative = true}});  // Encodeur 7 -> CC 7, Canal 1

    controlsMapping.emplace_back(
        InputMapping{.controlId = 78,
                     .mappingType = MappingType::ENCODER,
                     .midiMapping = {.channel = 0,
                                     .control = 8,
                                     .isRelative = true}});  // Encodeur 8 -> CC 8, Canal 1

    controlsMapping.emplace_back(
        InputMapping{.controlId = 80,
                     .mappingType = MappingType::ENCODER,
                     .midiMapping = {.channel = 0,
                                     .control = 10,
                                     .isRelative = true}});  // Encodeur optique -> CC 10, Canal 1

    controlsMapping.emplace_back(InputMapping{
        .controlId = 1071,
        .mappingType = MappingType::BUTTON,
        .midiMapping = {.channel = 0,
                        .control = 36,
                        .isRelative = true}});  // Bouton Encodeur 1 -> Note 36, Canal 1

    controlsMapping.emplace_back(InputMapping{
        .controlId = 1072,
        .mappingType = MappingType::BUTTON,
        .midiMapping = {.channel = 0,
                        .control = 37,
                        .isRelative = true}});  // Bouton Encodeur 2 -> Note 37, Canal 1

    controlsMapping.emplace_back(InputMapping{
        .controlId = 1073,
        .mappingType = MappingType::BUTTON,
        .midiMapping = {.channel = 0,
                        .control = 38,
                        .isRelative = true}});  // Bouton Encodeur 3 -> Note 38, Canal 1

    controlsMapping.emplace_back(InputMapping{
        .controlId = 1074,
        .mappingType = MappingType::BUTTON,
        .midiMapping = {.channel = 0,
                        .control = 39,
                        .isRelative = true}});  // Bouton Encodeur 4 -> Note 39, Canal 1

    controlsMapping.emplace_back(InputMapping{
        .controlId = 1075,
        .mappingType = MappingType::BUTTON,
        .midiMapping = {.channel = 0,
                        .control = 40,
                        .isRelative = true}});  // Bouton Encodeur 5 -> Note 40, Canal 1

    controlsMapping.emplace_back(InputMapping{
        .controlId = 1076,
        .mappingType = MappingType::BUTTON,
        .midiMapping = {.channel = 0,
                        .control = 41,
                        .isRelative = true}});  // Bouton Encodeur 6 -> Note 41, Canal 1

    controlsMapping.emplace_back(InputMapping{
        .controlId = 1077,
        .mappingType = MappingType::BUTTON,
        .midiMapping = {.channel = 0,
                        .control = 42,
                        .isRelative = true}});  // Bouton Encodeur 7 -> Note 42, Canal 1

    controlsMapping.emplace_back(InputMapping{
        .controlId = 1078,
        .mappingType = MappingType::BUTTON,
        .midiMapping = {.channel = 0,
                        .control = 43,
                        .isRelative = true}});  // Bouton Encodeur 8 -> Note 43, Canal 1

    // Boutons de navigation
    controlsMapping.emplace_back(
        InputMapping{.controlId = 51,
                     .mappingType = MappingType::BUTTON,
                     .midiMapping = {.channel = 0,
                                     .control = 44,
                                     .isRelative = true}});  // Bouton Home -> Note 44, Canal 1

    controlsMapping.emplace_back(
        InputMapping{.controlId = 52,
                     .mappingType = MappingType::BUTTON,
                     .midiMapping = {.channel = 0,
                                     .control = 45,
                                     .isRelative = true}});  // Bouton Retour -> Note 45, Canal 1

    // Encodeur de navigation (pour quand il n'est pas utilisé pour la navigation)
    controlsMapping.emplace_back(InputMapping{
        .controlId = 79,
        .mappingType = MappingType::ENCODER,
        .midiMapping = {.channel = 0,
                        .control = 9,
                        .isRelative = true}});  // Encodeur de navigation, sert de selecteur

    controlsMapping.emplace_back(InputMapping{
        .controlId = 79,
        .mappingType = MappingType::BUTTON,
        .midiMapping = {.channel = 0,
                        .control = 46,
                        .isRelative = true}});  // Bouton encodeur sert de bouton de validation
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
    midiMappingIndex.clear();  // S'assurer que l'index est vide

    for (size_t i = 0; i < controlsMapping.size(); ++i) {
        InputId controlId = controlsMapping[i].controlId;
        MappingType controlType = controlsMapping[i].mappingType;

        // Utilisation d'une clé composite pour distinguer les contrôles du même ID
        // mais de types différents (encoder vs button)
        uint32_t compositeKey =
            (static_cast<uint32_t>(controlId) << 8) | static_cast<uint8_t>(controlType);
        midiMappingIndex[compositeKey] = i;
    }
}