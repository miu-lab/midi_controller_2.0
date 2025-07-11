#pragma once

#include <lvgl.h>
#include <functional>
#include "config/UIConstants.hpp"

/**
 * @brief Builder pour créer des pages de menu LVGL de manière cohérente
 * 
 * Cette classe élimine la duplication de code dans LvglMenuView en fournissant
 * des méthodes réutilisables pour créer différents types d'éléments de menu.
 */
class MenuPageBuilder {
public:
    explicit MenuPageBuilder(lv_obj_t* menu);
    
    // ====================
    // CRÉATION DE PAGES
    // ====================
    
    /**
     * @brief Crée une nouvelle page de menu avec titre optionnel
     */
    lv_obj_t* createPage(const char* title = nullptr);
    
    /**
     * @brief Crée une section dans une page
     */
    lv_obj_t* createSection(lv_obj_t* page);
    
    // ====================
    // TYPES D'ÉLÉMENTS
    // ====================
    
    /**
     * @brief Crée un élément de menu simple avec label
     */
    lv_obj_t* createLabelItem(lv_obj_t* section, const char* text);
    
    /**
     * @brief Crée un élément avec switch (on/off)
     */
    lv_obj_t* createSwitchItem(lv_obj_t* section, const char* label, bool checked = false);
    
    /**
     * @brief Crée un élément avec slider
     */
    lv_obj_t* createSliderItem(lv_obj_t* section, const char* label, int value = 50, int min = 0, int max = 100);
    
    /**
     * @brief Crée un élément de navigation vers une sous-page
     */
    lv_obj_t* createNavigationItem(lv_obj_t* section, const char* label, lv_obj_t* target_page);
    
    // ====================
    // CONFIGURATION & STYLE
    // ====================
    
    /**
     * @brief Applique le style standard aux éléments de menu
     */
    void applyStandardItemStyle(lv_obj_t* item);
    
    /**
     * @brief Configure un élément pour être focusable via groupe LVGL
     */
    void makeFocusable(lv_obj_t* item);
    
private:
    lv_obj_t* menu_;
    
    // ====================
    // MÉTHODES PRIVÉES
    // ====================
    
    /**
     * @brief Crée un conteneur d'élément de menu de base
     */
    lv_obj_t* createBaseItem(lv_obj_t* section);
    
    /**
     * @brief Applique la configuration de base à une section
     */
    void configureSectionDefaults(lv_obj_t* section);
    
    /**
     * @brief Applique la configuration de base à une page
     */
    void configurePageDefaults(lv_obj_t* page);
};

/**
 * @brief Factory pour créer des pages complètes avec leurs éléments
 * 
 * Cette classe utilise MenuPageBuilder pour créer des pages entières
 * basées sur des configurations définies.
 */
class MenuPageFactory {
public:
    explicit MenuPageFactory(MenuPageBuilder& builder);
    
    // ====================
    // PAGES PRÉDÉFINIES
    // ====================
    
    lv_obj_t* createWiFiPage(lv_obj_t* parent_page);
    lv_obj_t* createBluetoothPage(lv_obj_t* parent_page);
    lv_obj_t* createAudioPage(lv_obj_t* parent_page);
    lv_obj_t* createInputPage(lv_obj_t* parent_page);
    lv_obj_t* createDisplayPage(lv_obj_t* parent_page);
    lv_obj_t* createAboutPage(lv_obj_t* parent_page);
    
private:
    MenuPageBuilder& builder_;
};