#pragma once

#include <cstdint>
#include <climits>
#include "core/domain/navigation/NavigationAction.hpp"

/**
 * @brief Constantes centralisées pour tout le système
 * 
 * Unifie toutes les constantes précédemment dispersées dans :
 * - UIConstants.hpp
 * - NavigationConstants.hpp  
 * - InputConstants.hpp
 * - UISystemConstants.hpp
 * 
 * Organisation par domaine fonctionnel pour éviter les doublons.
 */
namespace SystemConstants {
    
    // ====================
    // TIMING ET PERFORMANCE GLOBAUX
    // ====================
    
    namespace Timing {
        // Timing principal système (60 FPS)
        constexpr uint32_t SYSTEM_UPDATE_INTERVAL_MS = 16;
        
        // Timeouts spécialisés
        constexpr uint32_t EVENT_PROCESSING_TIMEOUT_MS = 5;
        constexpr uint32_t DISPLAY_REFRESH_TIMEOUT_MS = 10;
        constexpr uint32_t VIEW_MANAGER_TIMEOUT_MS = 8;
        constexpr uint32_t NAVIGATION_TIMEOUT_MS = 12;
        constexpr uint32_t INPUT_PROCESSING_TIMEOUT_MS = 3;
        
        // Timings UI spécifiques
        constexpr uint32_t MODAL_DEFAULT_TIMEOUT_MS = 3000;
        constexpr uint32_t BUTTON_DEBOUNCE_MS = 50;
        constexpr uint32_t ENCODER_RATE_LIMIT_MS = 1;
    }
    
    // ====================
    // ENCODEURS (UNIFIÉ)
    // ====================
    
    namespace Encoders {
        // Sensibilité uniforme
        constexpr float DEFAULT_SENSITIVITY = 1.0f;
        constexpr float MIN_SENSITIVITY = 0.1f;
        constexpr float MAX_SENSITIVITY = 5.0f;
        
        // Configuration hardware
        constexpr int32_t STEPS_PER_DETENT = 4;
        constexpr int32_t MIN_DELTA_THRESHOLD = 1;
        constexpr int32_t MAX_DELTA_VALUE = INT_MAX;
        constexpr int32_t MIN_DELTA_VALUE = INT_MIN;
        
        // Actions par défaut
        constexpr NavigationAction DEFAULT_ACTION = NavigationAction::ITEM_NAVIGATOR;
        constexpr int DEFAULT_PARAMETER = 0;
    }
    
    // ====================
    // BOUTONS (UNIFIÉ)  
    // ====================
    
    namespace Buttons {
        // Actions par défaut
        constexpr NavigationAction DEFAULT_ACTION = NavigationAction::ITEM_VALIDATE;
        constexpr int DEFAULT_PARAMETER = 0;
        constexpr bool PROCESS_PRESS_ONLY = true;
        constexpr bool PROCESS_RELEASE_TOO = false;
    }
    
    // ====================
    // NAVIGATION (UNIFIÉ)
    // ====================
    
    namespace Navigation {
        // IDs de contrôles spéciaux
        constexpr uint8_t OK_BUTTON_ID = 52;
        constexpr uint8_t BACK_BUTTON_ID = 51;
        constexpr uint8_t HOME_BUTTON_ID = 50;
        constexpr uint8_t MAIN_ENCODER_ID = 1;
        
        // Paramètres de contexte
        constexpr int ROOT_PAGE_CONTEXT = 0;
        constexpr int SUB_PAGE_CONTEXT = 1;
        constexpr int PARAMETER_EDIT_CONTEXT = 2;
        constexpr int MODAL_CONTEXT = 3;
        
        // Limites système
        constexpr uint8_t MAX_MENU_DEPTH = 5;
        constexpr uint8_t MAX_ITEMS_PER_PAGE = 10;
        constexpr uint8_t MAX_VIEW_STACK_DEPTH = 8;
    }
    
    // ====================
    // INTERFACE UTILISATEUR (UNIFIÉ)
    // ====================
    
    namespace UI {
        // Configuration système
        constexpr bool DEFAULT_ENABLE_FULL_UI = true;
        constexpr bool DEFAULT_ENABLE_DISPLAY_REFRESH = true;
        constexpr bool DEFAULT_ENABLE_EVENT_PROCESSING = true;
        
        // Dimensions
        constexpr int16_t SCREEN_WIDTH = 320;
        constexpr int16_t SCREEN_HEIGHT = 240;
        constexpr int16_t MENU_ITEM_HEIGHT = 30;
        constexpr int16_t MENU_PADDING = 5;
        constexpr int16_t HEADER_HEIGHT = 40;
        
        // Couleurs
        constexpr uint32_t COLOR_BLACK = 0x000000;
        constexpr uint32_t COLOR_WHITE = 0xFFFFFF;
        constexpr uint32_t COLOR_GREEN_NEON = 0x96FC6A;
        constexpr uint32_t COLOR_BLUE_DARK = 0x001122;
        constexpr uint32_t COLOR_GRAY = 0x808080;
        
        // Limites
        constexpr uint32_t MAX_MESSAGE_LENGTH = 256;
        constexpr uint32_t MAX_CONCURRENT_MODALS = 1;
        constexpr uint32_t EVENT_QUEUE_SIZE = 64;
    }
    
    // ====================
    // AUDIO ET MIDI (UNIFIÉ)
    // ====================
    
    namespace Audio {
        // Sensibilité audio (différente des encodeurs UI)
        constexpr uint8_t AUDIO_SENSITIVITY_DEFAULT = 50;
        constexpr uint8_t AUDIO_SENSITIVITY_MIN = 1;
        constexpr uint8_t AUDIO_SENSITIVITY_MAX = 100;
        
        // Volume
        constexpr uint8_t VOLUME_DEFAULT = 75;
        constexpr uint8_t VOLUME_MIN = 0;
        constexpr uint8_t VOLUME_MAX = 100;
        
        // MIDI
        constexpr uint8_t MIDI_CHANNEL_DEFAULT = 1;
        constexpr uint8_t MIDI_CC_DEFAULT = 1;
        constexpr uint8_t MIDI_NOTE_DEFAULT = 60; // C4
    }
    
    // ====================
    // VALIDATION (UNIFIÉ)
    // ====================
    
    namespace Validation {
        constexpr bool STRICT_INPUT_VALIDATION = true;
        constexpr bool STRICT_COMPONENT_VALIDATION = true;
        constexpr bool ALLOW_PARTIAL_INITIALIZATION = false;
        constexpr bool ALLOW_ZERO_SENSITIVITY = false;
        constexpr bool ENABLE_OVERFLOW_PROTECTION = true;
        constexpr bool VALIDATE_EVENT_BUS_REQUIRED = true;
        
        // Limites entrées
        constexpr int32_t MAX_ABSOLUTE_POSITION = INT_MAX;
        constexpr int8_t MAX_RELATIVE_CHANGE = 127;
        constexpr int8_t MIN_RELATIVE_CHANGE = -128;
    }
    
    // ====================
    // MESSAGES D'ERREUR (UNIFIÉ)
    // ====================
    
    namespace ErrorMessages {
        // Erreurs de composants  
        constexpr const char* NULL_UNIFIED_CONFIG = "UnifiedConfiguration is null";
        constexpr const char* NULL_EVENT_BUS = "EventBus is null";
        constexpr const char* NULL_STATE_MANAGER = "NavigationStateManager is null";
        constexpr const char* NULL_VIEW_MANAGER = "ViewManager is null";
        constexpr const char* NULL_DISPLAY_MANAGER = "DisplayManager is null";
        
        // Erreurs opérationnelles
        constexpr const char* SYSTEM_NOT_OPERATIONAL = "System not operational";
        constexpr const char* ALREADY_INITIALIZED = "System already initialized";
        constexpr const char* COMPONENTS_MISSING = "Required components missing";
        constexpr const char* INVALID_CONFIGURATION = "Invalid system configuration";
        
        // Erreurs de validation
        constexpr const char* CONTROL_NOT_FOUND = "Control definition not found";
        constexpr const char* INVALID_SENSITIVITY = "Invalid sensitivity value";
        constexpr const char* OVERFLOW_DETECTED = "Parameter overflow detected";
        constexpr const char* INVALID_ACTION = "Invalid navigation action";
    }
    
    // ====================
    // DEBUG ET LOGGING (UNIFIÉ)
    // ====================
    
    namespace Debug {
        // Activation globale par domaine
        constexpr bool ENABLE_SYSTEM_LOGGING = false;
        constexpr bool ENABLE_INPUT_LOGGING = false;
        constexpr bool ENABLE_NAVIGATION_LOGGING = false;
        constexpr bool ENABLE_UI_LOGGING = false;
        constexpr bool ENABLE_AUDIO_LOGGING = false;
        
        // Préfixes de logs
        constexpr const char* LOG_PREFIX_SYSTEM = "[SYSTEM]";
        constexpr const char* LOG_PREFIX_INPUT = "[INPUT]";
        constexpr const char* LOG_PREFIX_NAVIGATION = "[NAV]";
        constexpr const char* LOG_PREFIX_UI = "[UI]";
        constexpr const char* LOG_PREFIX_AUDIO = "[AUDIO]";
    }
    
    // ====================
    // LABELS INTERFACE (UNIFIÉ)
    // ====================
    
    namespace Labels {
        // Menu principal
        constexpr const char* MAIN_MENU = "Main Menu";
        constexpr const char* WIFI_SETTINGS = "WiFi Settings";
        constexpr const char* BLUETOOTH_SETTINGS = "Bluetooth Settings";
        constexpr const char* AUDIO_SETTINGS = "Audio Settings";
        constexpr const char* MIDI_SETTINGS = "MIDI Settings";
        constexpr const char* SYSTEM_SETTINGS = "System Settings";
        
        // Paramètres communs
        constexpr const char* VOLUME = "Volume";
        constexpr const char* SENSITIVITY = "Sensitivity";
        constexpr const char* ENCODER_SENSITIVITY = "Encoder Sensitivity";
        constexpr const char* BACK = "Back";
        constexpr const char* OK = "OK";
        constexpr const char* CANCEL = "Cancel";
    }
};