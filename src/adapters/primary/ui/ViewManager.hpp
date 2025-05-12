#pragma once
#include <Arduino.h>

/**
 * @brief Gestionnaire de vues pour l'interface utilisateur
 *
 * Cette classe est responsable de la gestion des différentes vues
 * et de la navigation entre elles.
 */
class ViewManager {
public:
    /**
     * @brief Entre dans un menu
     */
    virtual void enterMenu() = 0;

    /**
     * @brief Sort d'un menu
     */
    virtual void exitMenu() = 0;

    /**
     * @brief Sélectionne l'élément de menu suivant
     */
    virtual void selectNextMenuItem() = 0;

    /**
     * @brief Sélectionne l'élément de menu précédent
     */
    virtual void selectPreviousMenuItem() = 0;

    /**
     * @brief Sélectionne directement un élément de menu
     * @param index Index de l'élément à sélectionner
     */
    virtual void selectMenuItem(int index) = 0;

    /**
     * @brief Obtient l'index du menu actuellement sélectionné
     * @return Index du menu actuel, ou -1 si aucun
     */
    virtual int getCurrentMenuIndex() const = 0;

    /**
     * @brief Vérifie si l'utilisateur est dans un menu
     * @return true si dans un menu, false sinon
     */
    virtual bool isInMenu() const = 0;

    /**
     * @brief Affiche l'écran principal
     */
    virtual void showMainScreen() = 0;

    /**
     * @brief Affiche le moniteur de contrôles
     */
    virtual void showControlMonitor() = 0;

    /**
     * @brief Affiche l'écran de débogage
     */
    virtual void showDebugScreen() = 0;

    /**
     * @brief Affiche une boîte de dialogue modale
     * @param message Message à afficher
     */
    virtual void showModalDialog(const String& message) = 0;

    /**
     * @brief Masque la boîte de dialogue modale
     */
    virtual void hideModalDialog() = 0;

    /**
     * @brief Bascule entre les boutons de la boîte de dialogue
     */
    virtual void toggleModalDialogButton() = 0;

    /**
     * @brief Vérifie si le bouton OK est sélectionné dans la boîte de dialogue
     * @return true si OK est sélectionné, false sinon
     */
    virtual bool isModalDialogOkSelected() const = 0;

    /**
     * @brief Fait défiler l'écran principal
     * @param delta Quantité de défilement
     */
    virtual void scrollMainScreenByDelta(int8_t delta) = 0;

    /**
     * @brief Fait défiler le moniteur de contrôles
     * @param delta Quantité de défilement
     */
    virtual void scrollControlMonitorByDelta(int8_t delta) = 0;

    /**
     * @brief Fait défiler les logs de débogage
     * @param delta Quantité de défilement
     */
    virtual void scrollDebugLogByDelta(int8_t delta) = 0;

    /**
     * @brief Met à jour les informations de contrôle sur le moniteur
     * @param controlId ID du contrôle source
     * @param type Type de message ("CC", "Note On", etc.)
     * @param channel Canal MIDI
     * @param number Numéro (CC ou note)
     * @param value Valeur
     */
    virtual void updateControlMonitorInfo(uint8_t controlId, const String& type, 
                                         uint8_t channel, uint8_t number, uint8_t value) = 0;

    /**
     * @brief Met à jour la position d'un encodeur sur l'interface
     * @param encoderId ID de l'encodeur
     * @param position Position actuelle
     */
    virtual void updateEncoderPosition(uint8_t encoderId, int32_t position) = 0;

    /**
     * @brief Met à jour l'état du bouton d'un encodeur sur l'interface
     * @param encoderId ID de l'encodeur
     * @param pressed État du bouton (pressé ou non)
     */
    virtual void updateEncoderButtonState(uint8_t encoderId, bool pressed) = 0;

    /**
     * @brief Met à jour l'état d'un bouton standard sur l'interface
     * @param buttonId ID du bouton
     * @param pressed État du bouton (pressé ou non)
     */
    virtual void updateButtonState(uint8_t buttonId, bool pressed) = 0;

    /**
     * @brief Destructeur virtuel
     */
    virtual ~ViewManager() = default;
};
