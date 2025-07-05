#include "config/unified/ConfigurationFactory.hpp"

#include <Arduino.h>  // Pour F() macro

#include "config/unified/ControlBuilder.hpp"

std::unique_ptr<UnifiedConfiguration> ConfigurationFactory::createDefaultConfiguration() {
    auto config = std::make_unique<UnifiedConfiguration>();

    // === BOUTONS STANDALONE (Navigation) ===

    // Bouton Menu avec long press
    config->addControl(ControlBuilder(51, "menu_button")
                           .withLabel("Menu")
                           .inGroup("Navigation")
                           .withDescription("Bouton Menu")
                           .asButton(32, 30, ButtonMode::TOGGLE)
                           .withLongPress(1000)
                           .withNavigation("HOME", MappingControlType::BUTTON)
                           .withDisplayOrder(1)
                           .build());

    // Bouton OK/Validation
    config->addControl(ControlBuilder(52, "ok_button")
                           .withLabel("OK")
                           .inGroup("Navigation")
                           .withDescription("Bouton OK")
                           .asButton(31, 30, ButtonMode::MOMENTARY)
                           .withNavigation("BACK", MappingControlType::BUTTON)
                           .withDisplayOrder(2)
                           .build());

    // === ENCODEURS MIDI (Groupe: MIDI) ===

    // Encodeur 1 (partie encodeur)
    config->addControl(ControlBuilder(71, "encoder_1")
                           .withLabel("Enc 1")
                           .inGroup("MIDI")
                           .withDescription("Enc 1")
                           .withDisplayOrder(1)
                           .asRotaryEncoder(22, 23, 24)
                           .withAcceleration(true, 4)
                           .withMidiCC(1, 0, true)  // Encodeur -> CC 1
                           .build());

    // Encodeur 1 (partie bouton)
    config->addControl(ControlBuilder(1071, "encoder_1_button")
                           .withLabel("Enc 1 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 1")
                           .withDisplayOrder(1)
                           .asButton(21, 30)
                           .asChildOf(71)
                           .withMidiNote(36, 0)     // Bouton -> Note 36
                           .build());

    // Encodeur 2 (partie encodeur)
    config->addControl(ControlBuilder(72, "encoder_2")
                           .withLabel("Enc 2")
                           .inGroup("MIDI")
                           .withDescription("Enc 2")
                           .withDisplayOrder(2)
                           .asRotaryEncoder(19, 20, 24)
                           .withAcceleration(true, 4)
                           .withMidiCC(2, 0, true)  // CC 2
                           .build());

    // Encodeur 2 (partie bouton)
    config->addControl(ControlBuilder(1072, "encoder_2_button")
                           .withLabel("Enc 2 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 2")
                           .withDisplayOrder(2)
                           .asButton(18, 30)
                           .asChildOf(72)
                           .withMidiNote(37, 0)     // Note 37
                           .build());

    // Encodeur 3 (partie encodeur)
    config->addControl(ControlBuilder(73, "encoder_3")
                           .withLabel("Enc 3")
                           .inGroup("MIDI")
                           .withDescription("Enc 3")
                           .withDisplayOrder(3)
                           .asRotaryEncoder(16, 17, 24)
                           .withAcceleration(true, 4)
                           .withMidiCC(3, 0, true)
                           .build());

    // Encodeur 3 (partie bouton)
    config->addControl(ControlBuilder(1073, "encoder_3_button")
                           .withLabel("Enc 3 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 3")
                           .withDisplayOrder(3)
                           .asButton(15, 30)
                           .asChildOf(73)
                           .withMidiNote(38, 0)
                           .build());

    // Encodeur 4 (partie encodeur)
    // NOTE: Pin A changée de 13 vers 25 pour éviter conflit avec SCK du SPI
    config->addControl(ControlBuilder(74, "encoder_4")
                           .withLabel("Enc 4")
                           .inGroup("MIDI")
                           .withDescription("Enc 4")
                           .withDisplayOrder(4)
                           .asRotaryEncoder(5, 14, 24)
                           .withAcceleration(true, 4)
                           .withMidiCC(4, 0, true)
                           .build());

    // Encodeur 4 (partie bouton)
    config->addControl(ControlBuilder(1074, "encoder_4_button")
                           .withLabel("Enc 4 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 4")
                           .withDisplayOrder(4)
                           .asButton(41, 30)
                           .asChildOf(74)
                           .withMidiNote(39, 0)
                           .build());

    // Encodeur 5 (partie encodeur)
    config->addControl(ControlBuilder(75, "encoder_5")
                           .withLabel("Enc 5")
                           .inGroup("MIDI")
                           .withDescription("Enc 5")
                           .withDisplayOrder(5)
                           .asRotaryEncoder(39, 40, 24)
                           .withAcceleration(true, 4)
                           .withMidiCC(5, 0, true)
                           .build());

    // Encodeur 5 (partie bouton)
    config->addControl(ControlBuilder(1075, "encoder_5_button")
                           .withLabel("Enc 5 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 5")
                           .withDisplayOrder(5)
                           .asButton(38, 30)
                           .asChildOf(75)
                           .withMidiNote(40, 0)
                           .build());

    // Encodeur 6 (partie encodeur)
    config->addControl(ControlBuilder(76, "encoder_6")
                           .withLabel("Enc 6")
                           .inGroup("MIDI")
                           .withDescription("Enc 6")
                           .withDisplayOrder(6)
                           .asRotaryEncoder(36, 37, 24)
                           .withAcceleration(true, 4)
                           .withMidiCC(6, 0, true)
                           .build());

    // Encodeur 6 (partie bouton)
    config->addControl(ControlBuilder(1076, "encoder_6_button")
                           .withLabel("Enc 6 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 6")
                           .withDisplayOrder(6)
                           .asButton(35, 30)
                           .asChildOf(76)
                           .withMidiNote(41, 0)
                           .build());

    // Encodeur 7 (partie encodeur)
    config->addControl(ControlBuilder(77, "encoder_7")
                           .withLabel("Enc 7")
                           .inGroup("MIDI")
                           .withDescription("Enc 7")
                           .withDisplayOrder(7)
                           .asRotaryEncoder(33, 34, 24)
                           .withAcceleration(true, 4)
                           .withMidiCC(7, 0, true)
                           .build());

    // Encodeur 7 (partie bouton)
    config->addControl(ControlBuilder(1077, "encoder_7_button")
                           .withLabel("Enc 7 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 7")
                           .withDisplayOrder(7)
                           .asButton(30, 30)
                           .asChildOf(77)
                           .withMidiNote(42, 0)
                           .build());

    // Encodeur 8 (partie encodeur)
    config->addControl(ControlBuilder(78, "encoder_8")
                           .withLabel("Enc 8")
                           .inGroup("MIDI")
                           .withDescription("Enc 8")
                           .withDisplayOrder(8)
                           .asRotaryEncoder(28, 29, 24)
                           .withAcceleration(true, 4)
                           .withMidiCC(8, 0, true)
                           .build());

    // Encodeur 8 (partie bouton)
    config->addControl(ControlBuilder(1078, "encoder_8_button")
                           .withLabel("Enc 8 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 8")
                           .withDisplayOrder(8)
                           .asButton(27, 30)
                           .asChildOf(78)
                           .withMidiNote(43, 0)
                           .build());

    // === ENCODEUR DE NAVIGATION ===
    // NOTE: Pins changées de 9,10 vers 4,5 pour éviter conflit avec l'écran (CS=9, DC=10)

    // Encodeur Navigation (partie encodeur)
    config->addControl(
        ControlBuilder(79, "nav_encoder")
            .withLabel("Navigation")
            .inGroup("Navigation")
            .withDescription("Encodeur Navigation")
            .withDisplayOrder(9)
            .asRotaryEncoder(25, 24, 96)
            .withSensitivity(1.5f)
            .withAcceleration(true, 4)
            .withNavigation("ITEM_NAVIGATOR", MappingControlType::ENCODER)
            .build());

    // Encodeur Navigation (partie bouton)
    config->addControl(
        ControlBuilder(1079, "nav_encoder_button")
            .withLabel("Nav Btn")
            .inGroup("Navigation")
            .withDescription("Bouton Navigation")
            .withDisplayOrder(9)
            .asButton(26, 30)
            .asChildOf(79)
            .withNavigation("ITEM_VALIDATE", MappingControlType::BUTTON)
            .build());

    // === ENCODEUR OPTIQUE ===

    // Encodeur Optique (pas de bouton)
    config->addControl(ControlBuilder(80, "optical_encoder")
                           .withLabel("Precision")
                           .inGroup("Precision")
                           .withDescription("Encodeur Precision")
                           .withDisplayOrder(10)
                           .asRotaryEncoder(2, 3, 600)
                           .withSensitivity(0.1f)
                           .withAcceleration(false)
                           .withMidiCC(10, 0, true)
                           .build());

    // Validation finale
    auto validationResult = config->validate();
    if (validationResult.isError()) {
        // Configuration validation failed - log error in production
        // In embedded environment, we continue with invalid config
    }

    return config;
}