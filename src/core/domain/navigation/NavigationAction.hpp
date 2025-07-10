#pragma once

#include <cstdint>

/**
 * @brief Actions de navigation typées pour remplacer les strings
 * 
 * Cette enum centralise toutes les actions de navigation possibles,
 * garantissant la cohérence et évitant les erreurs de typage.
 */
enum class NavigationAction : uint8_t {
    // === NAVIGATION GÉNÉRALE ===
    HOME = 0,              ///< Retour à l'accueil
    BACK,                  ///< Retour arrière
    MENU_ENTER,            ///< Entrer dans un menu
    MENU_EXIT,             ///< Sortir d'un menu
    
    // === NAVIGATION DANS LES LISTES ===
    ITEM_NAVIGATOR,        ///< Navigation dans une liste (encodeur)
    ITEM_VALIDATE,         ///< Validation d'un élément sélectionné
    ITEM_NEXT,             ///< Élément suivant
    ITEM_PREVIOUS,         ///< Élément précédent
    
    // === ÉDITION DE PARAMÈTRES ===
    PARAMETER_EDIT,        ///< Éditer un paramètre
    PARAMETER_VALIDATE,    ///< Valider l'édition
    PARAMETER_CANCEL,      ///< Annuler l'édition
    
    // === PROFILS ET CONTEXTES ===
    PROFILE_SWITCH,        ///< Changer de profil
    CONTEXT_SWITCH,        ///< Changer de contexte
    
    // === ACTIONS SPÉCIALES ===
    QUICK_ACTION_1,        ///< Action rapide 1
    QUICK_ACTION_2,        ///< Action rapide 2
    TOGGLE_VIEW            ///< Basculer entre vues
};

/**
 * @brief Paramètres optionnels pour une action de navigation
 */
struct NavigationActionSpec {
    NavigationAction action;
    int parameter = 0;           ///< Paramètre optionnel (ex: direction, index)
    uint8_t context = 0;         ///< Contexte spécifique
    
    NavigationActionSpec(NavigationAction act, int param = 0, uint8_t ctx = 0)
        : action(act), parameter(param), context(ctx) {}
};

/**
 * @brief Utilitaires pour les actions de navigation
 */
class NavigationActionUtils {
public:
    /**
     * @brief Convertit une action en string pour debug/logging
     */
    static const char* toString(NavigationAction action);
    
    /**
     * @brief Vérifie si une action est valide
     */
    static bool isValid(NavigationAction action);
};