#pragma once

#include "PerformanceDefines.h"

/**
 * @brief Configuration des performances et timings
 * 
 * Centralise tous les paramètres liés aux performances :
 * refresh rates, timeouts, limites de temps d'exécution.
 */
namespace PerformanceConfig {

// === REFRESH RATES (utilise PerformanceDefines.h) ===
constexpr int DISPLAY_REFRESH_RATE_HZ = LVGL_REFRESH_RATE_HZ;
constexpr int VSYNC_SPACING = 2;
constexpr int DIFF_GAP = 4;
constexpr unsigned long DISPLAY_REFRESH_PERIOD_MS = LVGL_REFRESH_PERIOD_MS;

// === TIMEOUTS D'EXÉCUTION ===
constexpr unsigned long MAX_UPDATE_TIME_US = 30000;  // 30ms cycle principal
constexpr unsigned long INPUT_TIME_INTERVAL = 2000;  // 1ms pour input
constexpr unsigned long MIDI_TIME_INTERVAL = 3000;   // 2ms pour MIDI
constexpr unsigned long NAVIGATION_TIME_INTERVAL = 10000; // 10ms pour navigation
constexpr unsigned long UI_TIME_INTERVAL = 16667;    // 16ms pour UI

// === RATE LIMITING ===
constexpr unsigned long ENCODER_RATE_LIMIT_MS = 1;  // Limite encodeurs à ~50Hz
constexpr unsigned long DUPLICATE_CHECK_MS = 1.5;   // Vérification doublons

// === MODES PERFORMANCE ===
constexpr bool PERFORMANCE_MODE_DEFAULT = true;     // Mode normal par défaut

// === TAILLES MAXIMALES ETL (pour éviter les allocations dynamiques) ===

// EventBus et système d'événements
constexpr size_t MAX_EVENT_SUBSCRIBERS = 32;        // Nombre max d'abonnés aux événements
constexpr size_t MAX_EVENT_QUEUE_SIZE = 64;         // Taille max de la queue d'événements

// MIDI - chemins critiques
constexpr size_t MAX_MIDI_CALLBACKS = 24;           // Max callbacks MIDI (CC, Note, etc.)
constexpr size_t MAX_MIDI_PENDING_PARAMS = 128;     // Max paramètres MIDI en attente de batch
constexpr size_t MAX_MIDI_MESSAGES_QUEUE = 256;     // Queue des messages MIDI entrants

// Configuration et contrôles
constexpr size_t MAX_CONTROL_DEFINITIONS = 64;      // Max définitions de contrôles
constexpr size_t MAX_MIDI_MAPPINGS = 128;           // Max mappings MIDI

// Navigation et UI
constexpr size_t MAX_NAVIGATION_ACTIONS = 32;       // Max actions de navigation
constexpr size_t MAX_UI_COMPONENTS = 16;            // Max composants UI actifs

// Pools d'objets
constexpr size_t EVENT_POOL_SIZE = 128;             // Taille du pool d'événements
constexpr size_t MIDI_EVENT_POOL_SIZE = 256;        // Pool spécialisé pour événements MIDI
constexpr size_t UI_EVENT_POOL_SIZE = 64;           // Pool pour événements UI

// === CONFIGURATION DE LA MÉMOIRE ===
constexpr size_t STACK_SIZE_BYTES = 8192;           // Taille stack par défaut
constexpr size_t HEAP_FRAGMENT_THRESHOLD = 1024;    // Seuil de fragmentation heap

// === CONFIGURATION DES PERFORMANCES TEMPS RÉEL ===
constexpr unsigned long MAX_MIDI_LATENCY_US = 1000;  // Latence MIDI max (1ms)
constexpr unsigned long MAX_UI_UPDATE_TIME_US = 16000; // Temps max mise à jour UI (16ms)
constexpr unsigned long MAX_INPUT_LATENCY_US = 500;    // Latence input max (0.5ms)

}  // namespace PerformanceConfig