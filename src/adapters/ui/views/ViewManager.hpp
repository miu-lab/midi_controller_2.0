#pragma once
#include <Arduino.h>

/**
 * @brief Types de vues disponibles
 */
enum class ViewType {
    SplashScreen,
    ParameterFocus,
    Menu,
    Modal
};

/**
 * @brief Gestionnaire de vues simplifié pour l'interface utilisateur
 *
 * Architecture simple avec seulement 3 vues principales :
 * - SplashScreen : Écran d'initialisation
 * - ParameterFocus : Vue par défaut pour affichage paramètres MIDI
 * - Menu : Navigation et configuration
 */
class ViewManager {
public:
    /**
     * @brief Initialise le gestionnaire de vues
     * @return true si l'initialisation a réussi, false sinon
     */
    virtual bool init() = 0;
    
    /**
     * @brief Met à jour toutes les vues actives
     */
    virtual void update() = 0;
    
    /**
     * @brief Rend les vues actives sur l'écran
     */
    virtual void render() = 0;

    /**
     * @brief Affiche la vue de focus paramètre (vue par défaut)
     * @param ccNumber Numéro de Control Change (0-127)
     * @param channel Canal MIDI (1-16)
     * @param value Valeur du paramètre (0-127)
     * @param parameterName Nom du paramètre
     */
    virtual void showParameterFocus(uint8_t ccNumber, uint8_t channel, uint8_t value, const String& parameterName) = 0;

    /**
     * @brief Met à jour la valeur dans la vue de focus paramètre
     * @param value Nouvelle valeur (0-127)
     */
    virtual void updateParameterValue(uint8_t value) = 0;

    /**
     * @brief Affiche le menu de navigation
     */
    virtual void showMenu() = 0;

    /**
     * @brief Retourne à la vue par défaut (ParameterFocus)
     */
    virtual void showHome() = 0;

    /**
     * @brief Affiche une boîte de dialogue modale
     * @param message Message à afficher
     */
    virtual void showModal(const String& message) = 0;

    /**
     * @brief Masque la boîte de dialogue modale
     */
    virtual void hideModal() = 0;

    /**
     * @brief Navigation dans le menu (encodeur)
     * @param direction Direction de navigation (+1 suivant, -1 précédent)
     */
    virtual void navigateMenu(int8_t direction) = 0;

    /**
     * @brief Sélection d'élément de menu (bouton/clic encodeur)
     */
    virtual void selectMenuItem() = 0;

    /**
     * @brief Vérifie si l'affichage a besoin d'être mis à jour
     * @return true si une mise à jour est nécessaire
     */
    virtual bool needsDisplayUpdate() const = 0;
    
    /**
     * @brief Réinitialise le flag de mise à jour
     */
    virtual void clearDisplayUpdateFlag() = 0;

    /**
     * @brief Destructeur virtuel
     */
    virtual ~ViewManager() = default;
};
