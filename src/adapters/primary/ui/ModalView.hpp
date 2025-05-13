#pragma once

#include <memory>
#include <Arduino.h>

#include "View.hpp"
#include "core/ports/output/DisplayPort.hpp"

/**
 * @brief Vue pour l'affichage de boîtes de dialogue modales
 */
class ModalView : public View {
public:
    /**
     * @brief Constructeur
     * @param display Pointeur vers l'affichage
     */
    explicit ModalView(std::shared_ptr<DisplayPort> display);
    
    /**
     * @brief Initialise la vue
     * @return true si l'initialisation a réussi, false sinon
     */
    bool init() override;
    
    /**
     * @brief Met à jour l'état de la vue
     */
    void update() override;
    
    /**
     * @brief Rend la vue sur l'affichage
     */
    void render() override;
    
    /**
     * @brief Gère les événements d'entrée
     * @param eventType Type d'événement
     * @param data Données d'événement
     * @return true si l'événement a été traité, false sinon
     */
    bool handleEvent(uint8_t eventType, int32_t data) override;
    
    /**
     * @brief Vérifie si la vue est active
     * @return true si la vue est active, false sinon
     */
    bool isActive() const override;
    
    /**
     * @brief Active ou désactive la vue
     * @param active Nouvel état d'activation
     */
    void setActive(bool active) override;
    
    /**
     * @brief Définit le message de la boîte de dialogue
     * @param message Message à afficher
     */
    void setMessage(const char* message);
    
    /**
     * @brief Définit le titre de la boîte de dialogue
     * @param title Titre à afficher
     */
    void setTitle(const char* title);
    
    /**
     * @brief Définit les étiquettes des boutons
     * @param okLabel Étiquette du bouton OK
     * @param cancelLabel Étiquette du bouton Cancel (vide = pas de bouton Cancel)
     */
    void setButtonLabels(const char* okLabel, const char* cancelLabel = "");
    
    /**
     * @brief Bascule la sélection entre les boutons
     */
    void toggleButton();
    
    /**
     * @brief Vérifie si le bouton OK est sélectionné
     * @return true si OK est sélectionné, false si Cancel est sélectionné
     */
    bool isOkSelected() const;

private:
    String message_;
    String title_;
    String okLabel_;
    String cancelLabel_;
    bool okSelected_;
    bool hasCancelButton_;
    
    /**
     * @brief Découpe le message en lignes pour l'affichage
     * @param message Message à découper
     * @return Tableau de lignes
     */
    std::vector<String> wrapText(const String& message);
};
