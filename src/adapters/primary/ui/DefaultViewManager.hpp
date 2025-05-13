#pragma once

#include <Arduino.h>
#include <memory>
#include <vector>

#include "ViewManager.hpp"
#include "View.hpp"
#include "MenuView.hpp"
#include "DebugView.hpp"
#include "ControlMonitorView.hpp"
#include "ContextualView.hpp"
#include "ModalView.hpp"
#include "core/ports/output/DisplayPort.hpp"

/**
 * @brief Implémentation par défaut du gestionnaire de vues
 * 
 * Cette classe gère un ensemble de vues et délègue les événements d'entrée
 * à la vue active appropriée.
 */
class DefaultViewManager : public ViewManager {
public:
    /**
     * @brief Constructeur avec dépendance sur l'affichage
     * @param display Référence à l'adaptateur d'affichage
     */
    explicit DefaultViewManager(std::shared_ptr<DisplayPort> display);
    
    /**
     * @brief Destructeur
     */
    ~DefaultViewManager() override = default;
    
    /**
     * @brief Initialise le gestionnaire de vues
     * @return true si l'initialisation a réussi, false sinon
     */
    bool init();
    
    /**
     * @brief Met à jour toutes les vues actives
     */
    void update();
    
    /**
     * @brief Rend les vues actives sur l'écran
     */
    void render();

    /**
     * @brief Entre dans un menu
     */
    void enterMenu() override;

    /**
     * @brief Sort d'un menu
     */
    void exitMenu() override;

    /**
     * @brief Sélectionne l'élément de menu suivant
     */
    void selectNextMenuItem() override;

    /**
     * @brief Sélectionne l'élément de menu précédent
     */
    void selectPreviousMenuItem() override;

    /**
     * @brief Sélectionne directement un élément de menu
     * @param index Index de l'élément à sélectionner
     */
    void selectMenuItem(int index) override;

    /**
     * @brief Obtient l'index du menu actuellement sélectionné
     * @return Index du menu actuel, ou -1 si aucun
     */
    int getCurrentMenuIndex() const override;

    /**
     * @brief Vérifie si l'utilisateur est dans un menu
     * @return true si dans un menu, false sinon
     */
    bool isInMenu() const override;

    /**
     * @brief Affiche l'écran principal
     */
    void showMainScreen() override;

    /**
     * @brief Affiche le moniteur de contrôles
     */
    void showControlMonitor() override;

    /**
     * @brief Affiche l'écran de débogage
     */
    void showDebugScreen() override;

    /**
     * @brief Affiche une boîte de dialogue modale
     * @param message Message à afficher
     */
    void showModalDialog(const String& message) override;

    /**
     * @brief Masque la boîte de dialogue modale
     */
    void hideModalDialog() override;

    /**
     * @brief Bascule entre les boutons de la boîte de dialogue
     */
    void toggleModalDialogButton() override;

    /**
     * @brief Vérifie si le bouton OK est sélectionné dans la boîte de dialogue
     * @return true si OK est sélectionné, false sinon
     */
    bool isModalDialogOkSelected() const override;

    /**
     * @brief Fait défiler l'écran principal
     * @param delta Quantité de défilement
     */
    void scrollMainScreenByDelta(int8_t delta) override;

    /**
     * @brief Fait défiler le moniteur de contrôles
     * @param delta Quantité de défilement
     */
    void scrollControlMonitorByDelta(int8_t delta) override;

    /**
     * @brief Fait défiler les logs de débogage
     * @param delta Quantité de défilement
     */
    void scrollDebugLogByDelta(int8_t delta) override;

    /**
     * @brief Met à jour les informations de contrôle sur le moniteur
     * @param controlId ID du contrôle source
     * @param type Type de message ("CC", "Note On", etc.)
     * @param channel Canal MIDI
     * @param number Numéro (CC ou note)
     * @param value Valeur
     */
    void updateControlMonitorInfo(uint8_t controlId, const String& type, 
                                 uint8_t channel, uint8_t number, uint8_t value) override;

    /**
     * @brief Met à jour la position d'un encodeur sur l'interface
     * @param encoderId ID de l'encodeur
     * @param position Position actuelle
     */
    void updateEncoderPosition(uint8_t encoderId, int32_t position) override;

    /**
     * @brief Met à jour l'état du bouton d'un encodeur sur l'interface
     * @param encoderId ID de l'encodeur
     * @param pressed État du bouton (pressé ou non)
     */
    void updateEncoderButtonState(uint8_t encoderId, bool pressed) override;

    /**
     * @brief Met à jour l'état d'un bouton standard sur l'interface
     * @param buttonId ID du bouton
     * @param pressed État du bouton (pressé ou non)
     */
    void updateButtonState(uint8_t buttonId, bool pressed) override;

private:
    std::shared_ptr<DisplayPort> display_;
    std::shared_ptr<MenuView> menuView_;
    std::shared_ptr<DebugView> debugView_;
    std::shared_ptr<ControlMonitorView> controlMonitorView_;
    std::shared_ptr<ContextualView> contextualView_;
    std::shared_ptr<ModalView> modalView_;
    
    std::vector<std::shared_ptr<View>> views_;
    std::shared_ptr<View> activeView_;
    
    bool initialized_ = false;
};
