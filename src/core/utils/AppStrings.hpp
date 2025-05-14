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
FLASH_STRING(MSG_MENU, "Menu");
FLASH_STRING(MSG_EXIT, "Exit");

// ============================================================================
// Messages d'erreur
// ============================================================================
FLASH_STRING(ERR_INIT_FAILED, "Initialization failed");
FLASH_STRING(ERR_DISPLAY_INIT_FAILED, "Display initialization failed");
FLASH_STRING(ERR_MIDI_INIT_FAILED, "MIDI initialization failed");
FLASH_STRING(ERR_CONFIG_INIT_FAILED, "Configuration initialization failed");
FLASH_STRING(ERR_INPUT_INIT_FAILED, "Input initialization failed");
FLASH_STRING(ERR_UI_INIT_FAILED, "UI initialization failed");
FLASH_STRING(ERR_INVALID_PARAMETER, "Invalid parameter");
FLASH_STRING(ERR_OUT_OF_MEMORY, "Out of memory");

// ============================================================================
// Noms des vues
// ============================================================================
FLASH_STRING(VIEW_MENU, "Menu");
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
FLASH_STRING(PERF_AVG, "Avg: %lu us");
FLASH_STRING(PERF_MAX, "Max: %lu us");
FLASH_STRING(PERF_MIN, "Min: %lu us");
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
// Messages de débogage
// ============================================================================
FLASH_STRING(DBG_EVENT_ENABLED_MIDI_OUT, "EventEnabledMidiOut decorator created");
FLASH_STRING(DBG_SENDING_CC, "####### EventEnabledMidiOut: Sending CC #######");
FLASH_STRING(DBG_SOURCE_ID, "Source ID: ");
FLASH_STRING(DBG_CHANNEL, "Channel: ");
FLASH_STRING(DBG_CC, "CC: ");
FLASH_STRING(DBG_VALUE, "Value: ");
FLASH_STRING(DBG_SEPARATOR, "############################################");

// ============================================================================
// Messages d'initialisation
// ============================================================================
FLASH_STRING(INIT_SSD1306, "SSD1306: Initializing display...");
FLASH_STRING(INIT_SSD1306_FAILED, "SSD1306 allocation failed");
FLASH_STRING(INIT_SSD1306_SUCCESS, "SSD1306: Display initialized successfully");
FLASH_STRING(INIT_SSD1306_READY, "SSD1306: Display ready");
FLASH_STRING(INIT_CREATING_DISPLAY_TASK, "UISubsystem: Creating display task...");
FLASH_STRING(INIT_DISPLAY_TASK_ADDED, "UISubsystem: Display task added with index ");
FLASH_STRING(INIT_EVENT_LISTENER, "UISubsystem: Created and subscribed DisplayEventListener");
FLASH_STRING(INIT_VIEW_MANAGER_LISTENER, "UISubsystem: Created and subscribed ViewManagerEventListener");
FLASH_STRING(SSD1306_NOT_INITIALIZED, "SSD1306: Update called but display not initialized");
