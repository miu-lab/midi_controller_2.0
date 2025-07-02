#pragma once

// ============================================================================
// DEBUG COMPLÈTEMENT DÉSACTIVÉ - Toutes les macros sont vides
// ============================================================================

// Macros de debug désactivées
#define DEBUG_LOG(level, format, ...) ((void)0)
#define DEBUG_ERROR(format, ...) ((void)0)
#define DEBUG_UI(format, ...) ((void)0)
#define DEBUG_MIDI(format, ...) ((void)0)
#define DEBUG_INPUT(format, ...) ((void)0)
#define DEBUG_ENCODER(format, ...) ((void)0)
#define DEBUG_BUTTON(format, ...) ((void)0)
#define DEBUG_SCHEDULER(format, ...) ((void)0)
#define DEBUG_SCHEDULER_VERBOSE(format, ...) ((void)0)
#define DEBUG_EVENT_BUS(format, ...) ((void)0)
#define DEBUG_INPUT_DETAIL(format, ...) ((void)0)
#define DEBUG_ENCODER_DETAIL(format, ...) ((void)0)
#define DEBUG_BUTTON_DETAIL(format, ...) ((void)0)
#define DEBUG_PRINTLN_FLASH(str) ((void)0)
#define DEBUG_PRINT_FLASH(str) ((void)0)

// ============================================================================
// CONSTANTES POUR COMPATIBILITÉ
// ============================================================================

// Variables nécessaires pour TaskScheduler
#define SCHEDULER_DEBUG_LEVEL 0
#define SCHEDULER_MSG_LEVEL 0