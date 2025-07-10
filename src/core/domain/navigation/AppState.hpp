#pragma once

#include <cstdint>

/**
 * @brief États principaux de l'application
 * 
 * Définit tous les états possibles de l'interface utilisateur
 * pour la navigation stateful.
 */
enum class AppState : uint8_t {
    SPLASH_SCREEN = 0,     ///< Écran d'initialisation
    PARAMETER_FOCUS,       ///< Vue par défaut (paramètres MIDI)
    MENU,                  ///< Menu de navigation
    PARAMETER_EDIT,        ///< Édition de paramètre
    MODAL_DIALOG,          ///< Boîte de dialogue modale
    DEBUG_VIEW,            ///< Vue de debug
    PROFILE_SELECTION      ///< Sélection de profil
};

/**
 * @brief Contexte d'état avec paramètres
 * 
 * Structure légère pour stocker l'état avec son contexte
 * dans l'historique de navigation.
 */
struct AppStateContext {
    AppState state;
    uint8_t parameter = 0;       ///< Paramètre contextuel (ex: ID menu)
    uint8_t subState = 0;        ///< Sous-état optionnel
    uint32_t timestamp = 0;      ///< Horodatage pour timeout (millis)
    
    AppStateContext(AppState s = AppState::SPLASH_SCREEN, uint8_t p = 0, uint8_t sub = 0)
        : state(s), parameter(p), subState(sub), timestamp(0) {}
    
    bool operator==(const AppStateContext& other) const {
        return state == other.state && parameter == other.parameter && subState == other.subState;
    }
    
    bool operator!=(const AppStateContext& other) const {
        return !(*this == other);
    }
};

/**
 * @brief Utilitaires pour les états d'application
 */
class AppStateUtils {
public:
    /**
     * @brief Convertit un état en string pour debug
     */
    static const char* toString(AppState state);
    
    /**
     * @brief Vérifie si un état est valide
     */
    static bool isValid(AppState state);
    
    /**
     * @brief Vérifie si la transition entre deux états est valide
     */
    static bool isValidTransition(AppState from, AppState to);
    
    /**
     * @brief Obtient l'état par défaut (fallback)
     */
    static AppState getDefaultState();
};