#pragma once

#include <cstdint>
#include <climits>
#include "core/domain/navigation/NavigationAction.hpp"

/**
 * @brief Constantes centralisées pour tout le système
 * 
 * Constantes centralisées après nettoyage - seules les constantes 
 * réellement utilisées dans le code sont conservées.
 * 
 * Organisation par domaine fonctionnel optimisée.
 */
namespace SystemConstants {
    
    // ====================
    // TIMING ET PERFORMANCE GLOBAUX
    // ====================
    
    namespace Timing {
        // Timing principal système (utilisé par BaseUIProcessor)
        constexpr uint32_t SYSTEM_UPDATE_INTERVAL_MS = 16;
        
        // Timeouts spécialisés (utilisés)
        constexpr uint32_t EVENT_PROCESSING_TIMEOUT_MS = 5;
        constexpr uint32_t DISPLAY_REFRESH_TIMEOUT_MS = 10;
        constexpr uint32_t VIEW_MANAGER_TIMEOUT_MS = 8;
    }
    
    // ====================
    // ENCODEURS (UNIFIÉ)
    // ====================
    
    namespace Encoders {
        // Sensibilité uniforme (utilisée)
        constexpr float DEFAULT_SENSITIVITY = 1.0f;
        
        // Configuration hardware (utilisée)
        constexpr int32_t MIN_DELTA_THRESHOLD = 1;
        constexpr int32_t MAX_DELTA_VALUE = INT_MAX;
        constexpr int32_t MIN_DELTA_VALUE = INT_MIN;
        
        // Actions par défaut (utilisées)
        constexpr NavigationAction DEFAULT_ACTION = NavigationAction::ITEM_NAVIGATOR;
        constexpr int DEFAULT_PARAMETER = 0;
    }
    
    // ====================
    // BOUTONS (UNIFIÉ)  
    // ====================
    
    namespace Buttons {
        // Actions par défaut (utilisées)
        constexpr NavigationAction DEFAULT_ACTION = NavigationAction::ITEM_VALIDATE;
        constexpr int DEFAULT_PARAMETER = 0;
        constexpr bool PROCESS_PRESS_ONLY = true;
    }
    
    
    // ====================
    // INTERFACE UTILISATEUR
    // ====================
    
    namespace UI {
        // Configuration système (utilisées)
        constexpr bool DEFAULT_ENABLE_DISPLAY_REFRESH = true;
        constexpr bool DEFAULT_ENABLE_EVENT_PROCESSING = true;
        constexpr bool SHOW_DEBUG_INFO = false;
        constexpr bool ENABLE_FULL_UI = true;
        
        // Dimensions (utilisées)
        constexpr int16_t MENU_PADDING = 5;
        
        // Couleurs (utilisées)
        constexpr uint32_t COLOR_BLACK = 0x000000;
        constexpr uint32_t COLOR_WHITE = 0xFFFFFF;
        constexpr uint32_t COLOR_GREEN_NEON = 0x96FC6A;
        
        // Limites (utilisées)
        constexpr uint32_t MAX_MESSAGE_LENGTH = 256;
    }
    
    // ====================
    // AUDIO ET MIDI
    // ====================
    
    namespace Audio {
        // Sensibilité audio (utilisée)
        constexpr uint8_t AUDIO_SENSITIVITY_DEFAULT = 50;
        
        // Volume (utilisé)
        constexpr uint8_t VOLUME_DEFAULT = 75;
        
        // MIDI (utilisées)
        constexpr uint8_t DEFAULT_CHANNEL = 0;  // Canal 1 (0-15)
        constexpr uint8_t CC_VALUE_MIN = 0;
        constexpr uint8_t CC_VALUE_MAX = 127;
        
        // Pools et limitations MIDI (utilisées)
        constexpr size_t COMMAND_POOL_SIZE = 4;
        constexpr size_t MAX_ACTIVE_NOTES = 16;
    }
    
    // ====================
    // VALIDATION
    // ====================
    
    namespace Validation {
        // Validation composants (utilisées)
        constexpr bool STRICT_COMPONENT_VALIDATION = true;
        constexpr bool VALIDATE_EVENT_BUS_REQUIRED = true;
        
        // Validation entrées (utilisées par processors)
        constexpr bool STRICT_INPUT_VALIDATION = true;
        constexpr bool ENABLE_OVERFLOW_PROTECTION = true;
        
        // Limites entrées (utilisées)
        constexpr int32_t MAX_ABSOLUTE_POSITION = INT_MAX;
        constexpr int8_t MAX_RELATIVE_CHANGE = 127;
        constexpr int8_t MIN_RELATIVE_CHANGE = -128;
    }
    
    // ====================
    // MESSAGES D'ERREUR
    // ====================
    
    namespace ErrorMessages {
        // Erreurs opérationnelles (utilisées)
        constexpr const char* SYSTEM_NOT_OPERATIONAL = "System not operational";
        constexpr const char* ALREADY_INITIALIZED = "System already initialized";
        constexpr const char* COMPONENTS_MISSING = "Required components missing";
        constexpr const char* NULL_VIEW_MANAGER = "ViewManager is null";
    }
    
    
    // ====================
    // HARDWARE
    // ====================
    
    namespace Hardware {
        // Communication série (utilisées)
        constexpr unsigned long SERIAL_BAUD_RATE = 115200;
        constexpr size_t MAX_COMMAND_HISTORY = 10;
        
        // SPI configuration (utilisée par Display::)
        constexpr uint32_t SPI_SPEED_DISPLAY = 40000000;   // 40MHz
    }
    
    // ====================
    // DISPLAY
    // ====================
    
    namespace Display {
        // Dimensions (remplace les duplicatas dans UI)
        constexpr uint16_t SCREEN_WIDTH = 320;
        constexpr uint16_t SCREEN_HEIGHT = 240;
        constexpr uint8_t ROTATION = 3;  // Paysage inversé
        
        // Buffers hardware
        constexpr size_t FRAMEBUFFER_SIZE = SCREEN_WIDTH * SCREEN_HEIGHT;
        constexpr size_t DIFFBUFFER_SIZE = 8192;
        
        // Buffers LVGL
        constexpr size_t LVGL_BUFFER_LINES = 30;
        constexpr size_t LVGL_BUFFER_SIZE = SCREEN_WIDTH * LVGL_BUFFER_LINES;
        
        // Pins hardware ILI9341
        constexpr uint8_t CS_PIN = 9;
        constexpr uint8_t DC_PIN = 10;
        constexpr uint8_t RST_PIN = 6;
        constexpr uint8_t MOSI_PIN = 11;
        constexpr uint8_t SCK_PIN = 13;
        constexpr uint8_t MISO_PIN = 12;
        
        // Communication SPI
        constexpr uint32_t SPI_SPEED = Hardware::SPI_SPEED_DISPLAY;
    }
    
    // ====================
    // INPUT
    // ====================
    
    namespace Input {
        // Configuration encodeurs (utilisée)
        constexpr float DEFAULT_ENCODER_SENSITIVITY = 1.0f;
    }
    
    // ====================
    // PERFORMANCE
    // ====================
    
    namespace Performance {
        // Refresh rates (utilisées)
        constexpr int DISPLAY_REFRESH_RATE_HZ = 120;
        constexpr unsigned long DISPLAY_REFRESH_PERIOD_MS = (1000 / DISPLAY_REFRESH_RATE_HZ);
        constexpr int VSYNC_SPACING = 2;
        constexpr int DIFF_GAP = 4;
        
        // Timeouts d'exécution (utilisés)
        constexpr unsigned long MAX_UPDATE_TIME_US = 30000;
        constexpr unsigned long INPUT_TIME_INTERVAL = 2000;
        constexpr unsigned long MIDI_TIME_INTERVAL = 3000;
        constexpr unsigned long UI_TIME_INTERVAL = 16667;
        
        // Rate limiting (utilisées)
        constexpr unsigned long DUPLICATE_CHECK_MS = 1.5;
        constexpr unsigned long ENCODER_RATE_LIMIT_MS = 5;
        constexpr bool PERFORMANCE_MODE_DEFAULT = true;
        
        // Tailles ETL (utilisées pour éviter allocations dynamiques)
        constexpr size_t MAX_EVENT_SUBSCRIBERS = 32;
        constexpr size_t MAX_MIDI_CALLBACKS = 24;
        constexpr size_t MAX_MIDI_PENDING_PARAMS = 128;
        constexpr size_t MAX_MIDI_MESSAGES_QUEUE = 256;
        constexpr size_t MAX_CONTROL_DEFINITIONS = 64;
        constexpr size_t MAX_MIDI_MAPPINGS = 128;
        constexpr size_t MAX_NAVIGATION_ACTIONS = 32;
        constexpr size_t MAX_UI_COMPONENTS = 16;
        
        // Pools d'objets (utilisés)
        constexpr size_t EVENT_POOL_SIZE = 128;
        constexpr size_t MIDI_EVENT_POOL_SIZE = 256;
        constexpr size_t UI_EVENT_POOL_SIZE = 64;
        
        // Performances temps réel (utilisées)
        constexpr unsigned long MAX_MIDI_LATENCY_US = 1000;
    }
    
    // ====================
    // LABELS INTERFACE
    // ====================
    
    namespace Labels {
        // Menu principal (utilisés)
        constexpr const char* WIFI_SETTINGS = "WiFi Settings";
        constexpr const char* BLUETOOTH_SETTINGS = "Bluetooth Settings";
        constexpr const char* AUDIO_SETTINGS = "Audio Settings";
        
        // Paramètres communs (utilisés)
        constexpr const char* ENCODER_SENSITIVITY = "Encoder Sensitivity";
    }
};