#pragma once

#include <memory>
#include <vector>
#include <string>
#include <Arduino.h>

#include "View.hpp"
#include "core/ports/output/DisplayPort.hpp"

/**
 * @brief Vue pour l'affichage et la gestion des menus
 */
class MenuView : public View {
public:
    /**
     * @brief Structure représentant un élément de menu
     */
    struct MenuItem {
        String label;
        int id;
        bool enabled;
        
        MenuItem(const String& l, int i, bool e = true) 
            : label(l), id(i), enabled(e) {}
    };
    
    /**
     * @brief Constructeur
     * @param display Pointeur vers l'affichage
     */
    explicit MenuView(std::shared_ptr<DisplayPort> display);
    
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
     * @brief Ajoute un élément au menu
     * @param label Étiquette de l'élément
     * @param id ID de l'élément
     * @param enabled Si l'élément est activé
     */
    void addItem(const String& label, int id, bool enabled = true);
    
    /**
     * @brief Sélectionne l'élément suivant
     */
    void selectNext();
    
    /**
     * @brief Sélectionne l'élément précédent
     */
    void selectPrevious();
    
    /**
     * @brief Sélectionne un élément par son index
     * @param index Index de l'élément à sélectionner
     */
    void selectItem(int index);
    
    /**
     * @brief Obtient l'index de l'élément sélectionné
     * @return Index de l'élément, ou -1 si aucun
     */
    int getCurrentIndex() const;
    
    /**
     * @brief Obtient l'ID de l'élément sélectionné
     * @return ID de l'élément, ou -1 si aucun
     */
    int getCurrentItemId() const;

    /**
     * @brief Définit le titre du menu
     * @param title Titre à afficher
     */
    void setTitle(const String& title);

private:
    std::vector<MenuItem> items_;
    int selectedIndex_;
    int scrollOffset_;
    int maxVisibleItems_;
    String title_;
    
    /**
     * @brief Met à jour le décalage de défilement si nécessaire
     */
    void updateScrollOffsetIfNeeded();
};
