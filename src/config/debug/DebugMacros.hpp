#pragma once

#include <Arduino.h>

#include "config/debug/DebugConfig.hpp"
#include "config/debug/SerialBuffer.hpp"

// Note: Les niveaux de débogage sont maintenant définis dans DebugConfig.hpp

// Niveaux spécifiques pour les composants
#define DEBUG_EVENT_BUS_LEVEL DEBUG_LEVEL_INFO
#define DEBUG_INPUT_LEVEL DEBUG_LEVEL_INFO
#define DEBUG_MIDI_LEVEL DEBUG_LEVEL_INFO
#define DEBUG_ENCODER_LEVEL DEBUG_LEVEL_INFO
#define DEBUG_BUTTONS_LEVEL DEBUG_LEVEL_INFO
#define DEBUG_UI_LEVEL DEBUG_LEVEL_INFO
#ifndef DEBUG_SCHEDULER_LEVEL  // Ne pas redéfinir si déjà défini
#define DEBUG_SCHEDULER_LEVEL DEBUG_LEVEL_INFO
#endif

// Vérifie si Serial est disponible (avec un timeout de 0ms)
inline bool isSerialReady() {
#ifdef DEBUG
    return Serial;  // Sur Teensy, Serial est toujours disponible après Serial.begin()
#else
    return false;
#endif
}

// Fonction pour ajouter un saut de ligne et limiter la longueur des messages
inline void formatDebugMessage(const String& msg) {
#ifdef DEBUG
    // Limite la longueur de la ligne à 80 caractères maximum
    String formattedMsg;
    if (msg.length() > 80) {
        formattedMsg = msg.substring(0, 77) + "...";
    } else {
        formattedMsg = msg;
    }

    // Affiche directement sur Serial pour éviter la dépendance circulaire
    Serial.println(formattedMsg);
#endif
}

// Macros de débogage optimisées pour la mémoire Flash
#ifdef DEBUG

// Macro générique avec niveau et ajout de saut de ligne
#define DEBUG_LOG(level, format, ...)                                        \
    do {                                                                     \
        if (DEBUG_LEVEL >= level && isSerialReady()) {                       \
            char buffer[100];                                                \
            snprintf_P(buffer, sizeof(buffer), PSTR(format), ##__VA_ARGS__); \
            formatDebugMessage(String(buffer));                              \
        }                                                                    \
    } while (0)

// Macros d'erreur (toujours disponibles en mode DEBUG)
#define DEBUG_ERROR(format, ...) DEBUG_LOG(DEBUG_LEVEL_ERROR, "[ERROR] " format, ##__VA_ARGS__)

// Macros spécifiques aux composants (conditionnelles selon la configuration)
#if defined(DEBUG_EVENT_BUS_ENABLED) && DEBUG_EVENT_BUS_ENABLED > 0
#ifndef DEBUG_EVENT_BUS  // Ne pas redéfinir si déjà défini ailleurs
#define DEBUG_EVENT_BUS(format, ...) DEBUG_LOG(DEBUG_EVENT_BUS_LEVEL, "[EB] " format, ##__VA_ARGS__)
#endif
#else
#ifndef DEBUG_EVENT_BUS  // Ne pas redéfinir si déjà défini ailleurs
#define DEBUG_EVENT_BUS(format, ...) \
    do {                             \
    } while (0)
#endif
#endif

#if defined(DEBUG_RAW_CONTROLS) && DEBUG_RAW_CONTROLS > 0
#define DEBUG_INPUT(format, ...) DEBUG_LOG(DEBUG_INPUT_LEVEL, "[INP] " format, ##__VA_ARGS__)
#define DEBUG_ENCODER(format, ...) DEBUG_LOG(DEBUG_ENCODER_LEVEL, "[ENC] " format, ##__VA_ARGS__)
#define DEBUG_BUTTON(format, ...) DEBUG_LOG(DEBUG_BUTTONS_LEVEL, "[BTN] " format, ##__VA_ARGS__)

// Débogage très détaillé uniquement si niveau > 1
#if DEBUG_RAW_CONTROLS > 1
#define DEBUG_INPUT_DETAIL(format, ...) \
    DEBUG_LOG(DEBUG_INPUT_LEVEL, "[INP+] " format, ##__VA_ARGS__)
#define DEBUG_ENCODER_DETAIL(format, ...) \
    DEBUG_LOG(DEBUG_ENCODER_LEVEL, "[ENC+] " format, ##__VA_ARGS__)
#define DEBUG_BUTTON_DETAIL(format, ...) \
    DEBUG_LOG(DEBUG_BUTTONS_LEVEL, "[BTN+] " format, ##__VA_ARGS__)
#else
#define DEBUG_INPUT_DETAIL(format, ...) \
    do {                                \
    } while (0)
#define DEBUG_ENCODER_DETAIL(format, ...) \
    do {                                  \
    } while (0)
#define DEBUG_BUTTON_DETAIL(format, ...) \
    do {                                 \
    } while (0)
#endif
#else
#define DEBUG_INPUT(format, ...) \
    do {                         \
    } while (0)
#define DEBUG_ENCODER(format, ...) \
    do {                           \
    } while (0)
#define DEBUG_BUTTON(format, ...) \
    do {                          \
    } while (0)
#define DEBUG_INPUT_DETAIL(format, ...) \
    do {                                \
    } while (0)
#define DEBUG_ENCODER_DETAIL(format, ...) \
    do {                                  \
    } while (0)
#define DEBUG_BUTTON_DETAIL(format, ...) \
    do {                                 \
    } while (0)
#endif

#define DEBUG_MIDI(format, ...) DEBUG_LOG(DEBUG_MIDI_LEVEL, "[MIDI] " format, ##__VA_ARGS__)
#define DEBUG_UI(format, ...) DEBUG_LOG(DEBUG_UI_LEVEL, "[UI] " format, ##__VA_ARGS__)

// Débogage pour l'ordonnanceur de tâches
// Niveau 0 : Désactivé
// Niveau 1 : Stats basiques
// Niveau 2 : Stats détaillées
#if defined(DEBUG_TASK_SCHEDULER_LEVEL) && DEBUG_TASK_SCHEDULER_LEVEL >= 1
#define DEBUG_SCHEDULER(format, ...) \
    DEBUG_LOG(DEBUG_SCHEDULER_LEVEL, "[SCHED] " format, ##__VA_ARGS__)

// Débogage détaillé pour l'ordonnanceur de tâches (statistiques détaillées)
#if DEBUG_TASK_SCHEDULER_LEVEL >= 2
#define DEBUG_SCHEDULER_VERBOSE(format, ...) \
    DEBUG_LOG(DEBUG_SCHEDULER_LEVEL, "[SCHED-DETAIL] " format, ##__VA_ARGS__)
#else
#define DEBUG_SCHEDULER_VERBOSE(format, ...) \
    do {                                     \
    } while (0)
#endif
#else
#define DEBUG_SCHEDULER(format, ...) \
    do {                             \
    } while (0)
#define DEBUG_SCHEDULER_VERBOSE(format, ...) \
    do {                                     \
    } while (0)
#endif

// Macros pour stocker les chaînes en mémoire Flash et les utiliser plus efficacement
#define DEBUG_PRINTLN_FLASH(str)           \
    do {                                   \
        if (isSerialReady()) {             \
            SerialBuffer::println(F(str)); \
        }                                  \
    } while (0)

#define DEBUG_PRINT_FLASH(str) \
    do {                       \
    } while (0)  // Version sans utilisation directe de Serial

#else  // DEBUG n'est pas défini
// Toutes les macros sont vides si DEBUG n'est pas défini
#define DEBUG_LOG(level, format, ...) \
    do {                              \
    } while (0)
#define DEBUG_ERROR(format, ...) \
    do {                         \
    } while (0)
#ifndef DEBUG_EVENT_BUS
#define DEBUG_EVENT_BUS(format, ...) \
    do {                             \
    } while (0)
#endif
#define DEBUG_INPUT(format, ...) \
    do {                         \
    } while (0)
#define DEBUG_ENCODER(format, ...) \
    do {                           \
    } while (0)
#define DEBUG_BUTTON(format, ...) \
    do {                          \
    } while (0)
#define DEBUG_INPUT_DETAIL(format, ...) \
    do {                                \
    } while (0)
#define DEBUG_ENCODER_DETAIL(format, ...) \
    do {                                  \
    } while (0)
#define DEBUG_BUTTON_DETAIL(format, ...) \
    do {                                 \
    } while (0)
#define DEBUG_MIDI(format, ...) \
    do {                        \
    } while (0)
#define DEBUG_UI(format, ...) \
    do {                      \
    } while (0)
#define DEBUG_SCHEDULER(format, ...) \
    do {                             \
    } while (0)
#define DEBUG_SCHEDULER_VERBOSE(format, ...) \
    do {                                     \
    } while (0)
#define DEBUG_PRINTLN_FLASH(str) \
    do {                         \
    } while (0)
#define DEBUG_PRINT_FLASH(str) \
    do {                       \
    } while (0)
#endif