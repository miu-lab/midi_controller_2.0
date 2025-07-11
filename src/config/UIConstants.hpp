#pragma once

#include <cstdint>
#include <cstddef>

namespace UIConstants {
    // ====================
    // DIMENSIONS & LAYOUT
    // ====================
    
    namespace Layout {
        constexpr int BORDER_WIDTH_FOCUSED = 5;
        constexpr int BORDER_WIDTH_NORMAL = 1;
        constexpr int PADDING_VERTICAL = 8;
        constexpr int PADDING_HORIZONTAL = 10;
        constexpr int MENU_ITEM_MARGIN = -10;  // Pour alignement à droite
    }
    
    namespace Sizes {
        constexpr int SLIDER_WIDTH = 120;
        constexpr int SLIDER_HEIGHT = 10;
        constexpr int BUTTON_INDICATOR_SIZE = 12;
        constexpr int ARC_RADIUS_DEFAULT = 70;
    }
    
    // ====================
    // COULEURS (pour migration vers UITheme)
    // ====================
    
    namespace Colors {
        // Couleurs principales - À migrer vers UITheme
        constexpr uint32_t BLACK = 0x000000;
        constexpr uint32_t WHITE = 0xFFFFFF;
        constexpr uint32_t GREEN_NEON = 0x96FC6A;   // Couleur de focus actuelle
        constexpr uint32_t DARK_GRAY = 0x333333;
        constexpr uint32_t GRAY = 0x606060;
        
        // États des boutons - À migrer vers UITheme  
        constexpr uint32_t BUTTON_INACTIVE = 0x606060;
        constexpr uint32_t BUTTON_ACTIVE = 0x00FF00;
        constexpr uint32_t BUTTON_PRESSED = 0x00AA00;
        constexpr uint32_t BUTTON_ERROR = 0xFF0000;
        
        // Arc et contrôles - À migrer vers UITheme
        constexpr uint32_t ARC_ACTIVE = 0x00FF88;
        constexpr uint32_t ARC_BACKGROUND = 0x333333;
        constexpr uint32_t ARC_KNOB_ACTIVE = 0x00AA00;
    }
    
    // ====================
    // TIMING & ANIMATION
    // ====================
    
    namespace Timing {
        constexpr uint32_t SPLASH_DURATION_MS = 2000;
        constexpr uint32_t DEBOUNCE_DEFAULT_MS = 30;
        constexpr uint32_t LONG_PRESS_MS = 1000;
        constexpr uint32_t ANIMATION_DURATION_MS = 200;
    }
    
    // ====================
    // NAVIGATION
    // ====================
    
    namespace Navigation {
        constexpr size_t MAX_HISTORY_SIZE = 8;
        constexpr uint8_t SUB_PAGE_CONTEXT_MARKER = 1;  // Parameter=1 pour différencier contextes
    }
    
    // ====================
    // SENSIBILITÉ & CALIBRATION
    // ====================
    
    namespace Input {
        constexpr float ENCODER_SENSITIVITY_DEFAULT = 1.0f;
        constexpr int32_t ENCODER_STEPS_PER_DETENT = 4;
        constexpr int32_t PARAMETER_DELTA_MIN = 1;  // Delta minimum pour éviter valeurs nulles
    }
    
    // ====================
    // AUDIO & PERFORMANCE
    // ====================
    
    namespace Audio {
        constexpr uint8_t MASTER_VOLUME_DEFAULT = 70;
        constexpr uint8_t ENCODER_SENSITIVITY_DEFAULT = 50;
        constexpr uint32_t SAMPLE_RATE_DEFAULT = 48000;
    }
    
    // ====================
    // AFFICHAGE
    // ====================
    
    namespace Display {
        constexpr uint8_t BRIGHTNESS_DEFAULT = 80;
        constexpr uint32_t AUTO_SLEEP_TIME_MS = 300000;  // 5 minutes
        constexpr bool AUTO_SLEEP_ENABLED_DEFAULT = true;
    }
    
    // ====================
    // TEXTES & LABELS
    // ====================
    
    namespace Labels {
        // Textes des menus principaux
        constexpr const char* WIFI_SETTINGS = "WiFi Settings";
        constexpr const char* BLUETOOTH_SETTINGS = "Bluetooth Settings";
        constexpr const char* AUDIO_SETTINGS = "Audio Settings";
        constexpr const char* INPUT_SETTINGS = "Input Settings";
        constexpr const char* DISPLAY_SETTINGS = "Display Settings";
        constexpr const char* ABOUT = "About";
        
        // Labels de configuration
        constexpr const char* ENABLE_WIFI = "Enable WiFi";
        constexpr const char* ENABLE_BLUETOOTH = "Enable Bluetooth";
        constexpr const char* MASTER_VOLUME = "Master Volume";
        constexpr const char* ENCODER_SENSITIVITY = "Encoder Sensitivity";
        constexpr const char* BRIGHTNESS = "Brightness";
        constexpr const char* AUTO_SLEEP = "Auto Sleep";
        
        // Informations système
        constexpr const char* DEVICE_NAME = "Device: MIDI Controller";
        constexpr const char* FIRMWARE_VERSION = "Firmware: v2.0.0";
        constexpr const char* HARDWARE_VERSION = "Hardware: Teensy 4.1";
    }
    
    // ====================
    // VALIDATION & LIMITES
    // ====================
    
    namespace Limits {
        constexpr int32_t PARAMETER_VALUE_MIN = 0;
        constexpr int32_t PARAMETER_VALUE_MAX = 127;
        constexpr uint8_t MIDI_CHANNEL_MIN = 1;
        constexpr uint8_t MIDI_CHANNEL_MAX = 16;
        constexpr uint8_t BRIGHTNESS_MIN = 10;
        constexpr uint8_t BRIGHTNESS_MAX = 100;
    }
}