#pragma once

#include "core/utils/FlashStrings.hpp"

/**
 * @brief Chaînes de caractères constantes stockées en mémoire Flash
 * 
 * Ce fichier centralise toutes les chaînes de caractères constantes du projet
 * pour faciliter la maintenance et la traduction.
 */

// ============================================================================
// Messages généraux
// ============================================================================
FLASH_STRING(MSG_UI_READY, "UI Ready");
FLASH_STRING(MSG_ERROR, "Error");
FLASH_STRING(MSG_SUCCESS, "Success");
FLASH_STRING(MSG_WARNING, "Warning");
FLASH_STRING(MSG_INFO, "Info");
FLASH_STRING(MSG_CONFIRM, "Confirm");
FLASH_STRING(MSG_CANCEL, "Cancel");
FLASH_STRING(MSG_OK, "OK");
FLASH_STRING(MSG_YES, "Yes");
FLASH_STRING(MSG_NO, "No");
FLASH_STRING(MSG_BACK, "Back");
FLASH_STRING(MSG_NEXT, "Next");
FLASH_STRING(MSG_SAVE, "Save");
FLASH_STRING(MSG_LOAD, "Load");
// Utiliser MENU pour les deux contextes (MSG et VIEW)
FLASH_STRING(MENU, "Menu");
FLASH_STRING(MSG_EXIT, "Exit");

// ============================================================================
// Messages d'erreur
// ============================================================================
// Format commun pour les erreurs d'initialisation
FLASH_STRING(FMT_INIT_FAILED, "%s initialization failed");
// Composants pour utiliser avec le format d'erreur d'initialisation
FLASH_STRING(COMP_DISPLAY, "Display");
FLASH_STRING(COMP_MIDI, "MIDI");
FLASH_STRING(COMP_CONFIG, "Configuration");
FLASH_STRING(COMP_INPUT, "Input");
FLASH_STRING(COMP_UI, "UI");
// Autres erreurs génériques
FLASH_STRING(ERR_INIT_FAILED, "Initialization failed");
FLASH_STRING(ERR_INVALID_PARAMETER, "Invalid parameter");
FLASH_STRING(ERR_OUT_OF_MEMORY, "Out of memory");

// ============================================================================
// Noms des vues
// ============================================================================
// VIEW_MENU utilise la définition commune MENU
FLASH_STRING(VIEW_DEBUG, "Debug");
FLASH_STRING(VIEW_CONTROL_MONITOR, "Control Monitor");
FLASH_STRING(VIEW_CONTEXTUAL, "Main");
FLASH_STRING(VIEW_MODAL, "Modal");
FLASH_STRING(VIEW_SPLASH_SCREEN, "Splash");
FLASH_STRING(VIEW_LAST_CONTROL, "Last Control");
FLASH_STRING(VIEW_PERFORMANCE, "Performance");

// ============================================================================
// Messages liés aux performances
// ============================================================================
FLASH_STRING(PERF_TITLE, "Display Performance:");
// Format commun pour les unités de mesure en microsecondes
FLASH_STRING(FMT_MICROSECONDS, "%lu us");
FLASH_STRING(FMT_AVG, "Avg: ");
FLASH_STRING(FMT_MAX, "Max: ");
FLASH_STRING(FMT_MIN, "Min: ");
FLASH_STRING(PERF_FPS, "FPS: %.1f");

// ============================================================================
// Messages liés au MIDI
// ============================================================================
FLASH_STRING(MIDI_CC, "CC");
FLASH_STRING(MIDI_NOTE_ON, "Note On");
FLASH_STRING(MIDI_NOTE_OFF, "Note Off");
FLASH_STRING(MIDI_PROG_CHANGE, "Prog Change");
FLASH_STRING(MIDI_PITCH_BEND, "Pitch Bend");
FLASH_STRING(MIDI_CHANNEL_PRESSURE, "Ch Press");
FLASH_STRING(MIDI_CONTROL, "Control");
FLASH_STRING(MIDI_VALUE, "Value");
FLASH_STRING(MIDI_CHANNEL, "Channel");

// ============================================================================
// Formats pour les vues
// ============================================================================
FLASH_STRING(FMT_ENCODER_ID, "Encoder: %d");
FLASH_STRING(FMT_BUTTON_ID, "Button: %d");
FLASH_STRING(FMT_TYPE_CH, "Type: %s  Ch: %d");
FLASH_STRING(FMT_NUMBER, "Number: %d");
FLASH_STRING(FMT_VALUE, "Value: %d");
FLASH_STRING(FMT_LAST_CONTROL_RENDER, "LastControlView rendering - ID=%d Val=%d");
FLASH_STRING(FMT_LAST_CONTROL_ACTIVE, "LastControlView: Setting active state to %s");
FLASH_STRING(STR_TRUE, "true");
FLASH_STRING(STR_FALSE, "false");
FLASH_STRING(MSG_CONTROL_UPDATED, "LastControlView: Control information updated");
FLASH_STRING(MSG_ACTIVATING_VIEW, "LastControlView: Activating view after control update");

// ============================================================================
// Messages de débogage
// ============================================================================
FLASH_STRING(DBG_EVENT_ENABLED_MIDI_OUT, "EventEnabledMidiOut created");
// Simplification des longs séparateurs
FLASH_STRING(DBG_SENDING_CC, "--- EventEnabledMidiOut: Sending CC ---");
FLASH_STRING(DBG_SEPARATOR, "-----------------------------------");
// Format compacte pour les étiquettes de débogage
FLASH_STRING(DBG_FMT, "%s:%d");
FLASH_STRING(DBG_SRC, "Src");
FLASH_STRING(DBG_CH, "Ch");
FLASH_STRING(DBG_CC, "CC");
FLASH_STRING(DBG_VAL, "Val");

// ============================================================================
// Messages d'initialisation
// ============================================================================
// Préfixes communs pour les modules
FLASH_STRING(PFX_SSD1306, "SSD1306:");
FLASH_STRING(PFX_UI, "UISubsystem:");
// Messages spécifiques avec formats compacts
FLASH_STRING(MSG_INIT_DISPLAY, " Initializing display...");
FLASH_STRING(MSG_ALLOC_FAILED, " allocation failed");
FLASH_STRING(MSG_INIT_SUCCESS, " Display initialized successfully");
FLASH_STRING(MSG_DISPLAY_READY, " Display ready");
FLASH_STRING(MSG_CREATING_TASK, " Creating display task...");
FLASH_STRING(MSG_TASK_ADDED, " Display task added with index ");
FLASH_STRING(MSG_EVENT_LISTENER, " Created and subscribed DisplayEventListener");
FLASH_STRING(MSG_VIEW_MANAGER, " Created and subscribed ViewManagerEventListener");
FLASH_STRING(MSG_NOT_INITIALIZED, " Update called but display not initialized");