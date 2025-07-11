#pragma once

#include <cstdint>
#include <climits>
#include "core/domain/navigation/NavigationAction.hpp"

namespace InputConstants {
    // ====================
    // ACTIONS PAR DÉFAUT
    // ====================
    
    namespace DefaultActions {
        constexpr NavigationAction ENCODER_DEFAULT = NavigationAction::ITEM_NAVIGATOR;
        constexpr NavigationAction BUTTON_DEFAULT = NavigationAction::ITEM_VALIDATE;
    }
    
    // ====================
    // SENSIBILITÉ ET CALCULS
    // ====================
    
    namespace Sensitivity {
        constexpr float DEFAULT_SENSITIVITY = 1.0f;
        constexpr float MIN_SENSITIVITY = 0.1f;
        constexpr float MAX_SENSITIVITY = 5.0f;
        constexpr int32_t MIN_DELTA_THRESHOLD = 1;  // Delta minimum pour mouvement réel
        constexpr int32_t MAX_DELTA_VALUE = INT_MAX;
        constexpr int32_t MIN_DELTA_VALUE = INT_MIN;
    }
    
    // ====================
    // PARAMÈTRES DE NAVIGATION
    // ====================
    
    namespace Navigation {
        constexpr int DEFAULT_BUTTON_PARAMETER = 0;
        constexpr int DEFAULT_ENCODER_PARAMETER = 0;
        constexpr bool PROCESS_BUTTON_PRESS_ONLY = true;  // Traiter seulement les appuis
        constexpr bool PROCESS_BUTTON_RELEASE_TOO = false;
    }
    
    // ====================
    // LIMITES SYSTÈME
    // ====================
    
    namespace Limits {
        constexpr int32_t MAX_ABSOLUTE_POSITION = INT_MAX;
        constexpr int8_t MAX_RELATIVE_CHANGE = 127;
        constexpr int8_t MIN_RELATIVE_CHANGE = -128;
    }
    
    // ====================
    // VALIDATION
    // ====================
    
    namespace Validation {
        constexpr bool STRICT_INPUT_VALIDATION = true;
        constexpr bool ALLOW_ZERO_SENSITIVITY = false;
        constexpr bool ENABLE_OVERFLOW_PROTECTION = true;
    }
    
    // ====================
    // MESSAGES D'ERREUR
    // ====================
    
    namespace ErrorMessages {
        constexpr const char* NULL_UNIFIED_CONFIG = "UnifiedConfiguration is null";
        constexpr const char* NULL_EVENT_BUS = "EventBus is null";
        constexpr const char* CONTROL_NOT_FOUND = "Control definition not found";
        constexpr const char* INVALID_SENSITIVITY = "Invalid sensitivity value";
        constexpr const char* OVERFLOW_DETECTED = "Parameter overflow detected";
    }
    
    // ====================
    // DEBUG & LOGGING
    // ====================
    
    namespace Debug {
        constexpr bool ENABLE_INPUT_LOGGING = false;  // Peut être activé pour debug
        constexpr bool ENABLE_SENSITIVITY_LOGGING = false;
        constexpr bool ENABLE_NAVIGATION_LOGGING = false;
        constexpr const char* LOG_PREFIX = "[INPUT]";
    }
};