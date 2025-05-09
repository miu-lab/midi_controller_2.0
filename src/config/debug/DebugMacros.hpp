#pragma once

#include <Arduino.h>
#include "config/debug/DebugConfig.hpp"

// Niveaux de débogage
#define DEBUG_LEVEL_NONE    0
#define DEBUG_LEVEL_ERROR   1
#define DEBUG_LEVEL_WARNING 2
#define DEBUG_LEVEL_INFO    3
#define DEBUG_LEVEL_DEBUG   4
#define DEBUG_LEVEL_TRACE   5

// Niveau de débogage global
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_LEVEL_INFO
#endif

// Niveaux spécifiques pour les composants
#define DEBUG_EVENT_BUS_LEVEL DEBUG_LEVEL_INFO
#define DEBUG_INPUT_LEVEL     DEBUG_LEVEL_INFO
#define DEBUG_MIDI_LEVEL      DEBUG_LEVEL_INFO
#define DEBUG_ENCODER_LEVEL   DEBUG_LEVEL_INFO
#define DEBUG_BUTTONS_LEVEL   DEBUG_LEVEL_INFO
#define DEBUG_UI_LEVEL        DEBUG_LEVEL_INFO
#define DEBUG_SCHEDULER_LEVEL DEBUG_LEVEL_INFO

// Vérifie si Serial est disponible (avec un timeout de 0ms)
inline bool isSerialReady() {
    return Serial;
}

// Macros de débogage optimisées pour la mémoire Flash
#ifdef DEBUG

// Macro générique avec niveau
#define DEBUG_LOG(level, format, ...) \
    do { if (DEBUG_LEVEL >= level && isSerialReady()) { Serial.printf_P(PSTR(format), ##__VA_ARGS__); } } while(0)

// Macros d'erreur (toujours disponibles en mode DEBUG)
#define DEBUG_ERROR(format, ...) DEBUG_LOG(DEBUG_LEVEL_ERROR, "[ERROR] " format, ##__VA_ARGS__)

// Macros spécifiques aux composants (conditionnelles selon la configuration)
#if defined(DEBUG_EVENT_BUS_ENABLED) && DEBUG_EVENT_BUS_ENABLED > 0
    #define DEBUG_EVENT_BUS(format, ...) DEBUG_LOG(DEBUG_EVENT_BUS_LEVEL, "[EB] " format, ##__VA_ARGS__)
#else
    #define DEBUG_EVENT_BUS(format, ...) do {} while(0)
#endif

#if defined(DEBUG_RAW_CONTROLS) && DEBUG_RAW_CONTROLS > 0
    #define DEBUG_INPUT(format, ...) DEBUG_LOG(DEBUG_INPUT_LEVEL, "[INP] " format, ##__VA_ARGS__)
    #define DEBUG_ENCODER(format, ...) DEBUG_LOG(DEBUG_ENCODER_LEVEL, "[ENC] " format, ##__VA_ARGS__)
    #define DEBUG_BUTTON(format, ...) DEBUG_LOG(DEBUG_BUTTONS_LEVEL, "[BTN] " format, ##__VA_ARGS__)
    
    // Débogage très détaillé uniquement si niveau > 1
    #if DEBUG_RAW_CONTROLS > 1
        #define DEBUG_INPUT_DETAIL(format, ...) DEBUG_LOG(DEBUG_INPUT_LEVEL, "[INP+] " format, ##__VA_ARGS__)
        #define DEBUG_ENCODER_DETAIL(format, ...) DEBUG_LOG(DEBUG_ENCODER_LEVEL, "[ENC+] " format, ##__VA_ARGS__)
        #define DEBUG_BUTTON_DETAIL(format, ...) DEBUG_LOG(DEBUG_BUTTONS_LEVEL, "[BTN+] " format, ##__VA_ARGS__)
    #else
        #define DEBUG_INPUT_DETAIL(format, ...) do {} while(0)
        #define DEBUG_ENCODER_DETAIL(format, ...) do {} while(0)
        #define DEBUG_BUTTON_DETAIL(format, ...) do {} while(0)
    #endif
#else
    #define DEBUG_INPUT(format, ...) do {} while(0)
    #define DEBUG_ENCODER(format, ...) do {} while(0)
    #define DEBUG_BUTTON(format, ...) do {} while(0)
    #define DEBUG_INPUT_DETAIL(format, ...) do {} while(0)
    #define DEBUG_ENCODER_DETAIL(format, ...) do {} while(0)
    #define DEBUG_BUTTON_DETAIL(format, ...) do {} while(0)
#endif

#define DEBUG_MIDI(format, ...) DEBUG_LOG(DEBUG_MIDI_LEVEL, "[MIDI] " format, ##__VA_ARGS__)
#define DEBUG_UI(format, ...) DEBUG_LOG(DEBUG_UI_LEVEL, "[UI] " format, ##__VA_ARGS__)

// Débogage pour l'ordonnanceur de tâches
#if defined(DEBUG_TASK_SCHEDULER) && DEBUG_TASK_SCHEDULER > 0
    #define DEBUG_SCHEDULER(format, ...) DEBUG_LOG(DEBUG_SCHEDULER_LEVEL, "[SCHED] " format, ##__VA_ARGS__)
#else
    #define DEBUG_SCHEDULER(format, ...) do {} while(0)
#endif

// Macros pour stocker les chaînes en mémoire Flash et les utiliser plus efficacement
#define DEBUG_PRINTLN_FLASH(str) \
    do { if (isSerialReady()) { Serial.println(F(str)); } } while(0)

#define DEBUG_PRINT_FLASH(str) \
    do { if (isSerialReady()) { Serial.print(F(str)); } } while(0)

#else // DEBUG n'est pas défini
// Toutes les macros sont vides si DEBUG n'est pas défini
#define DEBUG_LOG(level, format, ...) do {} while(0)
#define DEBUG_ERROR(format, ...) do {} while(0)
#define DEBUG_EVENT_BUS(format, ...) do {} while(0)
#define DEBUG_INPUT(format, ...) do {} while(0)
#define DEBUG_ENCODER(format, ...) do {} while(0)
#define DEBUG_BUTTON(format, ...) do {} while(0)
#define DEBUG_INPUT_DETAIL(format, ...) do {} while(0)
#define DEBUG_ENCODER_DETAIL(format, ...) do {} while(0)
#define DEBUG_BUTTON_DETAIL(format, ...) do {} while(0)
#define DEBUG_MIDI(format, ...) do {} while(0)
#define DEBUG_UI(format, ...) do {} while(0)
#define DEBUG_PRINTLN_FLASH(str) do {} while(0)
#define DEBUG_PRINT_FLASH(str) do {} while(0)
#endif
