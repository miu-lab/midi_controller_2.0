#pragma once

#include "PerformanceConfig.hpp"
#include <etl/vector.h>
#include <etl/flat_map.h>
#include <etl/unordered_map.h>
#include <etl/array.h>

/**
 * @brief Configuration ETL pour remplacer STL dans les chemins critiques
 * 
 * Ce fichier centralise les typedefs ETL avec tailles fixes pour éviter
 * les allocations dynamiques dans les chemins temps réel.
 */
namespace ETLConfig {

// === MIDI - Chemins critiques ===

// Callbacks MIDI avec tailles fixes
template<typename T>
using MidiCallbackVector = etl::vector<T, PerformanceConfig::MAX_MIDI_CALLBACKS>;

// Paramètres MIDI en attente (pour batching)
template<typename Key, typename Value>
using MidiPendingMap = etl::flat_map<Key, Value, PerformanceConfig::MAX_MIDI_PENDING_PARAMS>;

// === EventBus - Système d'événements ===

// Subscriptions EventBus
template<typename T>
using EventSubscriptionVector = etl::vector<T, PerformanceConfig::MAX_EVENT_SUBSCRIBERS>;

// === DependencyContainer - Injection de dépendances ===

// Map pour les dépendances (utilise flat_map pour performance)
template<typename Key, typename Value>
using DependencyMap = etl::unordered_map<Key, Value, PerformanceConfig::MAX_CONTROL_DEFINITIONS>;

// === Configuration et contrôles ===

// Définitions de contrôles
template<typename T>
using ControlDefinitionVector = etl::vector<T, PerformanceConfig::MAX_CONTROL_DEFINITIONS>;

// Mappings MIDI
template<typename Key, typename Value>
using MidiMappingMap = etl::flat_map<Key, Value, PerformanceConfig::MAX_MIDI_MAPPINGS>;

// === Navigation et UI ===

// Actions de navigation
template<typename T>
using NavigationActionVector = etl::vector<T, PerformanceConfig::MAX_NAVIGATION_ACTIONS>;

// Composants UI
template<typename T>
using UIComponentVector = etl::vector<T, PerformanceConfig::MAX_UI_COMPONENTS>;

// === Queues et buffers ===

// Queue des messages MIDI entrants
template<typename T>
using MidiMessageQueue = etl::vector<T, PerformanceConfig::MAX_MIDI_MESSAGES_QUEUE>;

// === Compteurs et statistiques ===

// Compteurs de performance avec taille fixe
template<typename T, size_t N = 16>
using PerformanceCounterArray = etl::array<T, N>;

} // namespace ETLConfig