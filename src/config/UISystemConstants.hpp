#pragma once

#include <cstdint>

/**
 * @brief Constantes centralisées pour le système UI
 * 
 * Centralise toutes les configurations et paramètres du système UI
 * pour améliorer la maintenabilité et la cohérence.
 */
namespace UISystemConstants {
    
    // ====================
    // CONFIGURATION SYSTÈME
    // ====================
    
    namespace System {
        constexpr bool DEFAULT_ENABLE_FULL_UI = true;
        constexpr bool DEFAULT_ENABLE_DISPLAY_REFRESH = true;
        constexpr bool DEFAULT_ENABLE_EVENT_PROCESSING = true;
        constexpr bool DEFAULT_ENABLE_DEBUG_MODE = false;
    }
    
    // ====================
    // TIMING ET PERFORMANCE
    // ====================
    
    namespace Timing {
        constexpr uint32_t UPDATE_INTERVAL_MS = 16;  // ~60 FPS
        constexpr uint32_t EVENT_PROCESSING_TIMEOUT_MS = 5;
        constexpr uint32_t DISPLAY_REFRESH_TIMEOUT_MS = 10;
        constexpr uint32_t VIEW_MANAGER_TIMEOUT_MS = 8;
    }
    
    // ====================
    // PRIORITÉS D'EXÉCUTION
    // ====================
    
    namespace Priority {
        enum class UpdateOrder : uint8_t {
            EVENTS = 1,      // Traiter les événements en premier
            VIEW_MANAGER = 2, // Puis mettre à jour les vues
            DISPLAY_REFRESH = 3      // Enfin rafraîchir l'affichage
        };
    }
    
    // ====================
    // VALIDATION
    // ====================
    
    namespace Validation {
        constexpr bool STRICT_COMPONENT_VALIDATION = true;
        constexpr bool ALLOW_PARTIAL_INITIALIZATION = false;
        constexpr bool VALIDATE_EVENT_BUS_REQUIRED = true;
    }
    
    // ====================
    // MESSAGES D'ERREUR
    // ====================
    
    namespace ErrorMessages {
        constexpr const char* UI_NOT_OPERATIONAL = "UI system not operational";
        constexpr const char* VIEWMANAGER_MISSING = "ViewManager not available";
        constexpr const char* DISPLAYMANAGER_MISSING = "DisplayManager required for display refresh";
        constexpr const char* EVENTBUS_MISSING = "EventBus required for event processing";
        constexpr const char* COMPONENTS_MISSING = "Required UI components missing";
        constexpr const char* LISTENER_REQUIRES_VIEWMANAGER = "ViewManager required for event listener";
        constexpr const char* ALREADY_INITIALIZED = "UI system already initialized";
    }
    
    // ====================
    // MODAL ET MESSAGES
    // ====================
    
    namespace Modal {
        constexpr uint32_t DEFAULT_MODAL_TIMEOUT_MS = 3000;
        constexpr bool AUTO_HIDE_MODALS = true;
        constexpr uint32_t MAX_MESSAGE_LENGTH = 256;
    }
    
    // ====================
    // DEBUG ET LOGGING
    // ====================
    
    namespace Debug {
        constexpr bool ENABLE_COMPONENT_LOGGING = false;
        constexpr bool ENABLE_UPDATE_CYCLE_LOGGING = false;
        constexpr bool ENABLE_EVENT_LOGGING = false;
        constexpr const char* LOG_PREFIX = "[UI_SYSTEM]";
    }
    
    // ====================
    // RESSOURCES SYSTÈME
    // ====================
    
    namespace Resources {
        constexpr uint32_t MAX_CONCURRENT_MODALS = 1;
        constexpr uint32_t EVENT_QUEUE_SIZE = 64;
        constexpr uint32_t MAX_VIEW_STACK_DEPTH = 8;
    }
};