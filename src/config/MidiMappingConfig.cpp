// src/config/MidiMappingConfig.cpp
#include "config/MidiMappingConfig.hpp"

// Définition de la table de mapping par défaut
const std::vector<MidiControlMapping> defaultMidiMappings = {
    
    // Devices 1-8 (Mecaniques)
    {.controlId = 71, {.channel = 1, .control = 10}},
    {.controlId = 72, {.channel = 1, .control = 11}}, 
    {.controlId = 73, {.channel = 1, .control = 12}}, 
    {.controlId = 74, {.channel = 1, .control = 13}},
    {.controlId = 75, {.channel = 1, .control = 14}}, 
    {.controlId = 76, {.channel = 1, .control = 15}}, 
    {.controlId = 77, {.channel = 1, .control = 16}}, 
    {.controlId = 78, {.channel = 1, .control = 17}},
    
    // Optique
    {.controlId = 79, {.channel = 1, .control = 18}}, 
    
    // Navigation
    {.controlId = 80, {.channel = 1, .control = 19}}  
};