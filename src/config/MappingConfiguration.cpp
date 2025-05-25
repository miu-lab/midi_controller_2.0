#include "config/MappingConfiguration.hpp"

#include <Arduino.h>

#include <algorithm>
#include <string>

MappingConfiguration::MappingConfiguration() {
    setupMapping();
    buildMappingIndex();
}

const MidiControl* MappingConfiguration::getMidiMappingForControl(
    InputId controlId, MappingControlType controlType) const {
    uint32_t compositeKey =
        (static_cast<uint32_t>(controlId) << 8) | static_cast<uint8_t>(controlType);
    auto it = mappingIndex.find(compositeKey);
    if (it != mappingIndex.end()) {
        return &controlsMapping[it->second].midiMapping;
    }
    return nullptr;
}

bool MappingConfiguration::isNavigationControl(InputId controlId) const {
    return std::find_if(controlsMapping.begin(),
                        controlsMapping.end(),
                        [controlId](const InputMapping& mapping) {
                            // Vérifie si le controlId correspond
                            if (mapping.controlId != controlId) {
                                return false;
                            }
                            // Vérifie si un des rôles est NAVIGATION
                            return std::any_of(
                                mapping.roles.begin(),
                                mapping.roles.end(),
                                [](MappingRole role) { return role == MappingRole::NAVIGATION; });
                        }) != controlsMapping.end();
}

const std::vector<InputMapping>& MappingConfiguration::getMappedControls() const {
    return controlsMapping;
}

const std::vector<NavigationControl>& MappingConfiguration::getNavigationControls() const {
    return navigationControls;
}

void MappingConfiguration::setupMapping() {
    // Reprise des configurations actuelles avec une meilleure organisation
    controlsMapping.clear();

    /*  CONTROLES MIDI  */

    /* CONTROLE MIDI 1/8
    Encodeurs rotatif mécanique avec bouton central
        [X][ ][ ][ ]
        [ ][ ][ ][ ]
    */
    controlsMapping.emplace_back(
        InputMapping{.controlId = 71,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::ENCODER,
                     .midiMapping = {.channel = 0, .control = 1, .isRelative = true}});
    controlsMapping.emplace_back(
        InputMapping{.controlId = 1071,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::BUTTON,
                     .midiMapping = {.channel = 0, .control = 36, .isRelative = true}});

    /* CONTROLE MIDI 2/8
    Encodeurs rotatif mécanique avec bouton central
        [ ][X][ ][ ]
        [ ][ ][ ][ ]
    */
    controlsMapping.emplace_back(
        InputMapping{.controlId = 72,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::ENCODER,
                     .midiMapping = {.channel = 0, .control = 2, .isRelative = true}});

    controlsMapping.emplace_back(
        InputMapping{.controlId = 1072,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::BUTTON,
                     .midiMapping = {.channel = 0, .control = 37, .isRelative = true}});

    /* CONTROLE MIDI 3/8
    Encodeurs rotatif mécanique avec bouton central
        [ ][ ][X][ ]
        [ ][ ][ ][ ]
    */
    controlsMapping.emplace_back(
        InputMapping{.controlId = 73,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::ENCODER,
                     .midiMapping = {.channel = 0, .control = 3, .isRelative = true}});

    controlsMapping.emplace_back(
        InputMapping{.controlId = 1073,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::BUTTON,
                     .midiMapping = {.channel = 0, .control = 38, .isRelative = true}});

    /* CONTROLE MIDI 4/8
    Encodeurs rotatif mécanique avec bouton central
        [ ][ ][ ][X]
        [ ][ ][ ][ ]
    */
    controlsMapping.emplace_back(
        InputMapping{.controlId = 74,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::ENCODER,
                     .midiMapping = {.channel = 0, .control = 4, .isRelative = true}});
    controlsMapping.emplace_back(
        InputMapping{.controlId = 1074,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::BUTTON,
                     .midiMapping = {.channel = 0, .control = 39, .isRelative = true}});

    /* CONTROLE MIDI 5/8
    Encodeurs rotatif mécanique avec bouton central
        [ ][ ][ ][ ]
        [X][ ][ ][ ]
    */
    controlsMapping.emplace_back(
        InputMapping{.controlId = 75,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::ENCODER,
                     .midiMapping = {.channel = 0, .control = 5, .isRelative = true}});
    controlsMapping.emplace_back(
        InputMapping{.controlId = 1075,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::BUTTON,
                     .midiMapping = {.channel = 0, .control = 40, .isRelative = true}});

    /* CONTROLE MIDI 6/8
    Encodeurs rotatif mécanique avec bouton central
        [ ][ ][ ][ ]
        [ ][X][ ][ ]
    */
    controlsMapping.emplace_back(
        InputMapping{.controlId = 76,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::ENCODER,
                     .midiMapping = {.channel = 0, .control = 6, .isRelative = true}});
    controlsMapping.emplace_back(
        InputMapping{.controlId = 1076,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::BUTTON,
                     .midiMapping = {.channel = 0, .control = 41, .isRelative = true}});

    /* CONTROLE MIDI 7/8
    Encodeurs rotatif mécanique avec bouton central
        [ ][ ][ ][ ]
        [ ][ ][X][ ]
    */
    controlsMapping.emplace_back(
        InputMapping{.controlId = 77,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::ENCODER,
                     .midiMapping = {.channel = 0, .control = 7, .isRelative = true}});
    controlsMapping.emplace_back(
        InputMapping{.controlId = 1077,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::BUTTON,
                     .midiMapping = {.channel = 0, .control = 42, .isRelative = true}});

    /* CONTROLE MIDI 8/8
    Encodeurs rotatif mécanique avec bouton central
        [ ][ ][ ][ ]
        [ ][ ][ ][X]
    */
    controlsMapping.emplace_back(
        InputMapping{.controlId = 78,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::ENCODER,
                     .midiMapping = {.channel = 0, .control = 8, .isRelative = true}});
    controlsMapping.emplace_back(
        InputMapping{.controlId = 1078,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::BUTTON,
                     .midiMapping = {.channel = 0, .control = 43, .isRelative = true}});

    /* CONTROLE MIDI OPTIQUE
    Encodeurs rotatif optique 600PPR
    */
    controlsMapping.emplace_back(
        InputMapping{.controlId = 80,
                     .roles = {MappingRole::MIDI},
                     .mappingType = MappingControlType::ENCODER,
                     .midiMapping = {.channel = 0, .control = 10, .isRelative = true}});

    /* CONTROLES DE NAVIGATION
     * Ces contrôles sont dédiés à la navigation dans l'interface utilisateur
     */

    // Bouton HOME
    controlsMapping.emplace_back(InputMapping{.controlId = 51,
                                              .roles = {MappingRole::NAVIGATION},
                                              .mappingType = MappingControlType::BUTTON,
                                              .navigationMapping = {.name = "HOME"}});
    // Bouton BACK
    controlsMapping.emplace_back(InputMapping{.controlId = 52,
                                              .roles = {MappingRole::NAVIGATION},
                                              .mappingType = MappingControlType::BUTTON,
                                              .navigationMapping = {.name = "BACK"}});

    // Encodeur de navigation
    controlsMapping.emplace_back(InputMapping{.controlId = 79,
                                              .roles = {MappingRole::NAVIGATION},
                                              .mappingType = MappingControlType::ENCODER,
                                              .navigationMapping = {.name = "ITEM_NAVIGATOR"}});
    // Bouton de validation de l'élément sélectionné
    controlsMapping.emplace_back(InputMapping{
        .controlId = 1079,
        .roles = {MappingRole::NAVIGATION},
        .mappingType = MappingControlType::BUTTON,
        .navigationMapping = {.name = "ITEM_VALIDATE"},
    });
}

void MappingConfiguration::buildMappingIndex() {
    // Construit un index pour un accès rapide aux mappings MIDI par ID de contrôle
    mappingIndex.clear();  // S'assurer que l'index est vide

    for (size_t i = 0; i < controlsMapping.size(); ++i) {
        InputId controlId = controlsMapping[i].controlId;
        MappingControlType controlType = controlsMapping[i].mappingType;

        // Utilisation d'une clé composite pour distinguer les contrôles du même ID
        // mais de types différents (encoder vs button)
        uint32_t compositeKey =
            (static_cast<uint32_t>(controlId) << 8) | static_cast<uint8_t>(controlType);
        mappingIndex[compositeKey] = i;
    }
}