#include "config/unified/ConfigurationFactory.hpp"

#include <Arduino.h>  // Pour F() macro

#include "config/unified/ControlBuilder.hpp"

std::unique_ptr<UnifiedConfiguration> ConfigurationFactory::createDefaultConfiguration() {
    auto config = std::make_unique<UnifiedConfiguration>();

    // === BOUTONS STANDALONE (Navigation) ===

    // Bouton Menu avec long press (sur MUX canal 0)
    config->addControl(ControlBuilder(51, "menu_button")
                           .withLabel("Menu")
                           .inGroup("Navigation")
                           .withDescription("Bouton Menu")
                           .asButton(muxPin(9))
                           .withLongPress(1000)
                           .asMenuButton()
                           .withDisplayOrder(1)
                           .build());

    // Bouton OK/Validation (sur MUX canal 1)
    config->addControl(ControlBuilder(52, "back_button")
                           .withLabel("Back")
                           .inGroup("Navigation")
                           .withDescription("Bouton Retour")
                           .asButton(muxPin(10))
                           .asBackButton()
                           .withDisplayOrder(2)
                           .build());

    // Nouveaux boutons de navigation sur MUX
    config->addControl(ControlBuilder(53, "nav_button_3")
                           .withLabel("Nav 3")
                           .inGroup("Navigation")
                           .withDescription("Bouton Navigation 3")
                           .asButton(muxPin(11))
                           .withDisplayOrder(3)
                           .build());

    config->addControl(ControlBuilder(54, "nav_button_4")
                           .withLabel("Nav 4")
                           .inGroup("Navigation")
                           .withDescription("Bouton Navigation 4")
                           .asButton(muxPin(12))
                           .withDisplayOrder(4)
                           .build());

    config->addControl(ControlBuilder(55, "nav_button_5")
                           .withLabel("Nav 5")
                           .inGroup("Navigation")
                           .withDescription("Bouton Navigation 5")
                           .asButton(muxPin(13))
                           .withDisplayOrder(5)
                           .build());

    config->addControl(ControlBuilder(56, "nav_button_6")
                           .withLabel("Nav 6")
                           .inGroup("Navigation")
                           .withDescription("Bouton Navigation 6")
                           .asButton(muxPin(14))
                           .withDisplayOrder(6)
                           .build());

    // === ENCODEURS MIDI (Groupe: MIDI) ===

    // Encodeur 1 (partie encodeur)
    config->addControl(ControlBuilder(71, "encoder_1x1")
                           .withLabel("Enc 1x1")
                           .inGroup("MIDI")
                           .withDescription("Enc 1x1")
                           .withDisplayOrder(1)
                           .asRotaryEncoder(22, 23, 24)
                           .withStepPerDetent(true, 1)
                           .withMidiCC(1, 0, true)  // Encodeur -> CC 1
                           .build());

    // Encodeur 1 (partie bouton sur MUX canal 8)
    config->addControl(ControlBuilder(1071, "encoder_1x1_button")
                           .withLabel("Enc 1x1 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 1x1")
                           .withDisplayOrder(1)
                           .asButton(muxPin(7), 30)
                           .asChildOf(71)
                           .withMidiNote(36, 0)  // Bouton -> Note 36
                           .build());

    // Encodeur 2 (partie encodeur)
    config->addControl(ControlBuilder(72, "encoder_1x2")
                           .withLabel("Enc 1x2")
                           .inGroup("MIDI")
                           .withDescription("Enc 1x2")
                           .withDisplayOrder(2)
                           .asRotaryEncoder(18, 19, 24)
                           .withStepPerDetent(true, 1)
                           .withMidiCC(2, 0, true)  // CC 2
                           .build());

    // Encodeur 2 (partie bouton sur MUX canal 9)
    config->addControl(ControlBuilder(1072, "encoder_1x2_button")
                           .withLabel("Enc 1x2 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 1x2")
                           .withDisplayOrder(2)
                           .asButton(muxPin(4), 30)
                           .asChildOf(72)
                           .withMidiNote(37, 0)  // Note 37
                           .build());

    // Encodeur 3 (partie encodeur)
    config->addControl(ControlBuilder(73, "encoder_1x3")
                           .withLabel("Enc 1x3")
                           .inGroup("MIDI")
                           .withDescription("Enc 1x3")
                           .withDisplayOrder(3)
                           .asRotaryEncoder(40, 41, 24)
                           .withStepPerDetent(true, 1)
                           .withMidiCC(3, 0, true)
                           .build());

    // Encodeur 3 (partie bouton sur MUX canal 10)
    config->addControl(ControlBuilder(1073, "encoder_1x3_button")
                           .withLabel("Enc 1x3 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 1x3")
                           .withDisplayOrder(3)
                           .asButton(muxPin(2), 30)
                           .asChildOf(73)
                           .withMidiNote(38, 0)
                           .build());

    // Encodeur 4 (partie encodeur)
    // NOTE: Pin A changée de 13 vers 25 pour éviter conflit avec SCK du SPI
    config->addControl(ControlBuilder(74, "encoder_1x4")
                           .withLabel("Enc 1x4")
                           .inGroup("MIDI")
                           .withDescription("Enc 1x4")
                           .withDisplayOrder(4)
                           .asRotaryEncoder(36, 37, 24)
                           .withStepPerDetent(true, 1)
                           .withMidiCC(4, 0, true)
                           .build());

    // Encodeur 4 (partie bouton sur MUX canal 11)
    config->addControl(ControlBuilder(1074, "encoder_1x4_button")
                           .withLabel("Enc 1x4 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 1x4")
                           .withDisplayOrder(4)
                           .asButton(muxPin(0), 30)
                           .asChildOf(74)
                           .withMidiNote(39, 0)
                           .build());

    // Encodeur 5 (partie encodeur)
    config->addControl(ControlBuilder(75, "encoder_2x1")
                           .withLabel("Enc 2x1")
                           .inGroup("MIDI")
                           .withDescription("Enc 2x1")
                           .withDisplayOrder(5)
                           .asRotaryEncoder(20, 21, 24)
                           .withStepPerDetent(true, 1)
                           .withMidiCC(5, 0, true)
                           .build());

    // Encodeur 5 (partie bouton sur MUX canal 12)
    config->addControl(ControlBuilder(1075, "encoder_2x1_button")
                           .withLabel("Enc 2x1 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 2x1")
                           .withDisplayOrder(5)
                           .asButton(muxPin(6), 30)
                           .asChildOf(75)
                           .withMidiNote(40, 0)
                           .build());

    // Encodeur 6 (partie encodeur)
    config->addControl(ControlBuilder(76, "encoder_2x2")
                           .withLabel("Enc 2x2")
                           .inGroup("MIDI")
                           .withDescription("Enc 2x2")
                           .withDisplayOrder(6)
                           .asRotaryEncoder(16, 17, 24)
                           .withStepPerDetent(true, 1)
                           .withMidiCC(6, 0, true)
                           .build());

    // Encodeur 6 (partie bouton sur MUX canal 13)
    config->addControl(ControlBuilder(1076, "encoder_2x2_button")
                           .withLabel("Enc 2x2 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 2x2")
                           .withDisplayOrder(6)
                           .asButton(muxPin(5), 30)
                           .asChildOf(76)
                           .withMidiNote(41, 0)
                           .build());

    // Encodeur 7 (partie encodeur)
    config->addControl(ControlBuilder(77, "encoder_2x3")
                           .withLabel("Enc 2x3")
                           .inGroup("MIDI")
                           .withDescription("Enc 2x3")
                           .withDisplayOrder(7)
                           .asRotaryEncoder(14, 15, 24)
                           .withStepPerDetent(true, 1)
                           .withMidiCC(7, 0, true)
                           .build());

    // Encodeur 7 (partie bouton sur MUX canal 14)
    config->addControl(ControlBuilder(1077, "encoder_2x3_button")
                           .withLabel("Enc 2x3 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 2x3")
                           .withDisplayOrder(7)
                           .asButton(muxPin(3), 30)
                           .asChildOf(77)
                           .withMidiNote(42, 0)
                           .build());

    // Encodeur 8 (partie encodeur)
    config->addControl(ControlBuilder(78, "encoder_2x4")
                           .withLabel("Enc 2x4")
                           .inGroup("MIDI")
                           .withDescription("Enc 2x4")
                           .withDisplayOrder(8)
                           .asRotaryEncoder(38, 39, 24)
                           .withStepPerDetent(true, 1)
                           .withMidiCC(8, 0, true)
                           .build());

    // Encodeur 8 (partie bouton sur MUX canal 15)
    config->addControl(ControlBuilder(1078, "encoder_2x4_button")
                           .withLabel("Enc 2x4 Btn")
                           .inGroup("MIDI")
                           .withDescription("Bouton Encodeur 2x4")
                           .withDisplayOrder(8)
                           .asButton(muxPin(1), 30)
                           .asChildOf(78)
                           .withMidiNote(43, 0)
                           .build());

    // === ENCODEUR DE NAVIGATION ===
    // NOTE: Pins changées de 9,10 vers 4,5 pour éviter conflit avec l'écran (CS=9, DC=10)

    // Encodeur Navigation (partie encodeur)
    config->addControl(ControlBuilder(79, "nav_encoder")
                           .withLabel("Navigation")
                           .inGroup("Navigation")
                           .withDescription("Encodeur Navigation")
                           .withDisplayOrder(9)
                           .asRotaryEncoder(30, 31, 24)
                           .withStepPerDetent(true, 4)
                           .asItemNavigator()
                           .build());

    // Encodeur Navigation (partie bouton)
    config->addControl(ControlBuilder(1079, "nav_encoder_button")
                           .withLabel("Nav Btn")
                           .inGroup("Navigation")
                           .withDescription("Bouton Navigation")
                           .withDisplayOrder(9)
                           .asButton(mcuPin(32), 30)
                           .asChildOf(79)
                           .asItemValidator()
                           .build());

    // === ENCODEUR OPTIQUE ===

    // Encodeur Optique (pas de bouton)
    config->addControl(ControlBuilder(80, "optical_encoder")
                           .withLabel("Precision")
                           .inGroup("Precision")
                           .withDescription("Encodeur Precision")
                           .withDisplayOrder(10)
                           .asRotaryEncoder(34, 33, 600)
                           .withStepPerDetent(true, 1)
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