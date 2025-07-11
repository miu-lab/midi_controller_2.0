#pragma once

#include <cstdint>
#include <cstddef>
#include "core/domain/navigation/NavigationAction.hpp"
#include "core/domain/navigation/AppState.hpp"

namespace NavigationConstants {
    // ====================
    // TAILLES & LIMITES
    // ====================
    
    constexpr size_t MAX_HISTORY_SIZE = 8;
    constexpr size_t ACTION_ROUTES_SIZE = 10;  // Taille du tableau ACTION_ROUTES
    
    // ====================
    // IDENTIFIANTS DE CONTRÔLES
    // ====================
    
    namespace ControlIds {
        constexpr uint16_t NAVIGATION_ENCODER = 79;
        constexpr uint16_t NAVIGATION_ENCODER_BUTTON = 1079;
        constexpr uint16_t OK_BUTTON = 52;
        constexpr uint8_t NAVIGATION_ENCODER_PIN = 8;
        constexpr uint8_t OK_BUTTON_PIN = 31;
    }
    
    // ====================
    // PARAMÈTRES DE CONTEXTE
    // ====================
    
    namespace Context {
        constexpr uint8_t SUB_PAGE_MARKER = 1;       // Parameter=1 pour marquer entrée sous-page
        constexpr uint8_t SUB_STATE_DIFFERENT = 1;   // subState=1 pour créer contexte différent
        constexpr uint8_t PARAMETER_DEFAULT = 0;
        constexpr uint8_t SUB_STATE_DEFAULT = 0;
    }
    
    // ====================
    // ACTIONS PAR DÉFAUT
    // ====================
    
    namespace DefaultActions {
        constexpr NavigationAction ENCODER_DEFAULT = NavigationAction::ITEM_NAVIGATOR;
        constexpr NavigationAction BUTTON_DEFAULT = NavigationAction::ITEM_VALIDATE;
    }
    
    // ====================
    // ÉTATS PAR DÉFAUT
    // ====================
    
    namespace DefaultStates {
        constexpr AppState INITIAL_STATE = AppState::SPLASH_SCREEN;
        constexpr AppState HOME_STATE = AppState::PARAMETER_FOCUS;
        constexpr AppState FALLBACK_STATE = AppState::PARAMETER_FOCUS;
    }
    
    // ====================
    // TIMING DE NAVIGATION
    // ====================
    
    namespace Timing {
        constexpr uint32_t STATE_TRANSITION_TIMEOUT_MS = 5000;  // Timeout pour transitions
        constexpr uint32_t CONTEXT_LIFETIME_MS = 300000;       // 5 minutes avant nettoyage auto
        constexpr uint32_t ENCODER_DEBOUNCE_MS = 10;           // Debounce pour encodeur
    }
    
    // ====================
    // CONFIGURATION ENCODEUR
    // ====================
    
    namespace EncoderConfig {
        constexpr int32_t STEPS_PER_DETENT = 4;
        constexpr float SENSITIVITY_DEFAULT = 1.0f;
        constexpr float SENSITIVITY_MIN = 0.1f;
        constexpr float SENSITIVITY_MAX = 5.0f;
        constexpr int32_t DELTA_MIN_THRESHOLD = 1;  // Delta minimum pour mouvement réel
    }
    
    // ====================
    // VALIDATION
    // ====================
    
    namespace Validation {
        constexpr bool STRICT_STATE_VALIDATION = true;
        constexpr bool ALLOW_SELF_TRANSITIONS = false;
        constexpr bool ENABLE_CONTEXT_CLEANUP = true;
    }
    
    // ====================
    // MESSAGES D'ERREUR
    // ====================
    
    namespace ErrorMessages {
        constexpr const char* INVALID_STATE_TRANSITION = "Invalid state transition attempted";
        constexpr const char* ACTION_NOT_VALID = "Action not valid in current context";
        constexpr const char* HISTORY_FULL = "Navigation history is full";
        constexpr const char* NULL_STATE_MANAGER = "StateManager is null";
        constexpr const char* NULL_EVENT_BUS = "EventBus is null";
    }
    
    // ====================
    // DEBUG & LOGGING
    // ====================
    
    namespace Debug {
        constexpr bool ENABLE_NAVIGATION_LOGGING = false;  // Peut être activé pour debug
        constexpr bool ENABLE_STATE_LOGGING = false;
        constexpr bool ENABLE_TRANSITION_LOGGING = false;
        constexpr const char* LOG_PREFIX = "[NAV]";
    }
}