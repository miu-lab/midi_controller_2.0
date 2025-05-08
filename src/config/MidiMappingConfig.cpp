#include "config/MidiMappingConfig.hpp"

// Définition des mappings MIDI par défaut pour les contrôles
const MidiControlMapping defaultMidiMappings[] = {
    // Encodeurs (71-80)
    { .controlId = 71, .midiControl = { .channel = 0, .control = 1, .relative = false } },  // Encodeur 1 -> CC 1, Canal 1
    { .controlId = 72, .midiControl = { .channel = 0, .control = 2, .relative = false } },  // Encodeur 2 -> CC 2, Canal 1
    { .controlId = 73, .midiControl = { .channel = 0, .control = 3, .relative = false } },  // Encodeur 3 -> CC 3, Canal 1
    { .controlId = 74, .midiControl = { .channel = 0, .control = 4, .relative = false } },  // Encodeur 4 -> CC 4, Canal 1
    { .controlId = 75, .midiControl = { .channel = 0, .control = 5, .relative = false } },  // Encodeur 5 -> CC 5, Canal 1
    { .controlId = 76, .midiControl = { .channel = 0, .control = 6, .relative = false } },  // Encodeur 6 -> CC 6, Canal 1
    { .controlId = 77, .midiControl = { .channel = 0, .control = 7, .relative = false } },  // Encodeur 7 -> CC 7, Canal 1
    { .controlId = 78, .midiControl = { .channel = 0, .control = 8, .relative = false } },  // Encodeur 8 -> CC 8, Canal 1
    { .controlId = 79, .midiControl = { .channel = 0, .control = 9, .relative = false } },  // Encodeur 9 -> CC 9, Canal 1
    { .controlId = 80, .midiControl = { .channel = 0, .control = 10, .relative = false } }, // Encodeur 10 -> CC 10, Canal 1
    
    // Boutons encodeurs (71-80)
    { .controlId = 71, .midiControl = { .channel = 0, .control = 36, .relative = false } }, // Bouton Encodeur 1 -> Note 36, Canal 1
    { .controlId = 72, .midiControl = { .channel = 0, .control = 37, .relative = false } }, // Bouton Encodeur 2 -> Note 37, Canal 1
    { .controlId = 73, .midiControl = { .channel = 0, .control = 38, .relative = false } }, // Bouton Encodeur 3 -> Note 38, Canal 1
    { .controlId = 74, .midiControl = { .channel = 0, .control = 39, .relative = false } }, // Bouton Encodeur 4 -> Note 39, Canal 1
    { .controlId = 75, .midiControl = { .channel = 0, .control = 40, .relative = false } }, // Bouton Encodeur 5 -> Note 40, Canal 1
    { .controlId = 76, .midiControl = { .channel = 0, .control = 41, .relative = false } }, // Bouton Encodeur 6 -> Note 41, Canal 1
    { .controlId = 77, .midiControl = { .channel = 0, .control = 42, .relative = false } }, // Bouton Encodeur 7 -> Note 42, Canal 1
    { .controlId = 78, .midiControl = { .channel = 0, .control = 43, .relative = false } }, // Bouton Encodeur 8 -> Note 43, Canal 1
    { .controlId = 79, .midiControl = { .channel = 0, .control = 44, .relative = false } }, // Bouton Encodeur 9 -> Note 44, Canal 1
    
    // Boutons indépendants (51-60)
    { .controlId = 51, .midiControl = { .channel = 0, .control = 48, .relative = false } }, // Bouton 1 -> Note 48, Canal 1
    { .controlId = 52, .midiControl = { .channel = 0, .control = 49, .relative = false } }, // Bouton 2 -> Note 49, Canal 1
};

// Nombre d'éléments dans le tableau de mappings par défaut
const size_t defaultMidiMappingCount = sizeof(defaultMidiMappings) / sizeof(defaultMidiMappings[0]);
