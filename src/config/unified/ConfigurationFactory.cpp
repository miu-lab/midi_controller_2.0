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
                           .asButton(32, ButtonMode::TOGGLE)
                           .withLongPress(1000)
                           .withNavigation("HOME", MappingControlType::BUTTON)
                           .withDisplayOrder(1)
                           .build());

    // Bouton OK/Validation
    config->addControl(ControlBuilder(52, "ok_button")
                           .withLabel("OK")
                           .inGroup("Navigation")
                           .withDescription("Bouton OK")
                           .asButton(31, ButtonMode::MOMENTARY)
                           .withNavigation("BACK", MappingControlType::BUTTON)
                           .withDisplayOrder(2)
                           .build());

    // === ENCODEURS MIDI (Groupe: MIDI) ===

    // Encodeur 1
    config->addControl(ControlBuilder(71, "encoder_1")
                           .withLabel("Enc 1")
                           .inGroup("MIDI")
                           .withDescription("Encodeur Enc 1")
                           .withDisplayOrder(1)
                           .asEncoder(22, 23, 24)
                           .withEncoderButton(21, 30)
                           .withAcceleration(true, 4)
                           .withMidiCC(1, 0, true)  // Encodeur -> CC 1
                           .withMidiNote(36, 0)     // Bouton -> Note 36
                           .build());

    // Encodeur 2
    config->addControl(ControlBuilder(72, "encoder_2")
                           .withLabel("Enc 2")
                           .inGroup("MIDI")
                           .withDescription("Encodeur Enc 2")
                           .withDisplayOrder(2)
                           .asEncoder(19, 20, 24)
                           .withEncoderButton(18, 30)
                           .withAcceleration(true, 4)
                           .withMidiCC(2, 0, true)  // CC 2
                           .withMidiNote(37, 0)     // Note 37
                           .build());

    // Encodeur 3
    config->addControl(ControlBuilder(73, "encoder_3")
                           .withLabel("Enc 3")
                           .inGroup("MIDI")
                           .withDescription("Encodeur Enc 3")
                           .withDisplayOrder(3)
                           .asEncoder(16, 17, 24)
                           .withEncoderButton(15, 30)
                           .withAcceleration(true, 4)
                           .withMidiCC(3, 0, true)
                           .withMidiNote(38, 0)
                           .build());

    // Encodeur 4
    // NOTE: Pin A changée de 13 vers 25 pour éviter conflit avec SCK du SPI
    config->addControl(ControlBuilder(74, "encoder_4")
                           .withLabel("Enc 4")
                           .inGroup("MIDI")
                           .withDescription("Encodeur Enc 4")
                           .withDisplayOrder(4)
                           .asEncoder(14, 5, 24)  // Changé pin A de 13 vers 25
                           .withEncoderButton(41, 30)
                           .withAcceleration(true, 4)
                           .withMidiCC(4, 0, true)
                           .withMidiNote(39, 0)
                           .build());

    // Encodeur 5
    config->addControl(ControlBuilder(75, "encoder_5")
                           .withLabel("Enc 5")
                           .inGroup("MIDI")
                           .withDescription("Encodeur Enc 5")
                           .withDisplayOrder(5)
                           .asEncoder(39, 40, 24)
                           .withEncoderButton(38, 30)
                           .withAcceleration(true, 4)
                           .withMidiCC(5, 0, true)
                           .withMidiNote(40, 0)
                           .build());

    // Encodeur 6
    config->addControl(ControlBuilder(76, "encoder_6")
                           .withLabel("Enc 6")
                           .inGroup("MIDI")
                           .withDescription("Encodeur Enc 6")
                           .withDisplayOrder(6)
                           .asEncoder(36, 37, 24)
                           .withEncoderButton(35, 30)
                           .withAcceleration(true, 4)
                           .withMidiCC(6, 0, true)
                           .withMidiNote(41, 0)
                           .build());

    // Encodeur 7
    config->addControl(ControlBuilder(77, "encoder_7")
                           .withLabel("Enc 7")
                           .inGroup("MIDI")
                           .withDescription("Encodeur Enc 7")
                           .withDisplayOrder(7)
                           .asEncoder(33, 34, 24)
                           .withEncoderButton(30, 30)
                           .withAcceleration(true, 4)
                           .withMidiCC(7, 0, true)
                           .withMidiNote(42, 0)
                           .build());

    // Encodeur 8
    config->addControl(ControlBuilder(78, "encoder_8")
                           .withLabel("Enc 8")
                           .inGroup("MIDI")
                           .withDescription("Encodeur Enc 8")
                           .withDisplayOrder(8)
                           .asEncoder(28, 29, 24)
                           .withEncoderButton(27, 30)
                           .withAcceleration(true, 4)
                           .withMidiCC(8, 0, true)
                           .withMidiNote(43, 0)
                           .build());

    // === ENCODEUR DE NAVIGATION ===
    // NOTE: Pins changées de 9,10 vers 4,5 pour éviter conflit avec l'écran (CS=9, DC=10)

    config->addControl(
        ControlBuilder(79, "nav_encoder")
            .withLabel("Navigation")
            .inGroup("Navigation")
            .withDescription("Encodeur Navigation")
            .withDisplayOrder(9)
            .asEncoder(25, 24, 96)  // Changé de 9,10 vers 4,5 - Higher resolution
            .withEncoderButton(26, 30)
            .withSensitivity(1.5f)
            .withAcceleration(true, 4)
            .withNavigation("ITEM_NAVIGATOR", MappingControlType::ENCODER)  // Encodeur navigue
            .withNavigation("ITEM_VALIDATE", MappingControlType::BUTTON)    // Bouton valide
            .build());

    // === ENCODEUR OPTIQUE ===

    config->addControl(ControlBuilder(80, "optical_encoder")
                           .withLabel("Precision")
                           .inGroup("Precision")
                           .withDescription("Encodeur Precision")
                           .withDisplayOrder(10)
                           .asEncoder(2, 3, 600)
                           .withSensitivity(0.1f)
                           .withAcceleration(false)  // Pas d'accélération
                           .withMidiCC(10, 0, true)  // CC 10
                           .build());

    // Validation finale
    if (!config->validate()) {
        // Configuration validation failed - log error in production
        // In embedded environment, we continue with invalid config
    }

    return config;
}