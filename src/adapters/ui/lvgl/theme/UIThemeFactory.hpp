#pragma once

#include "UITheme.hpp"
#include "UILayout.hpp"
#include <memory>

/**
 * @brief Factory pour créer et configurer les thèmes UI
 * 
 * Cette classe fournit une interface simplifiée pour créer des thèmes
 * complets avec leurs layouts associés, optimisés pour différents
 * contextes d'utilisation du contrôleur MIDI.
 */
class UIThemeFactory {
public:
    /**
     * @brief Types de thèmes disponibles
     */
    enum class ThemeType {
        DEFAULT,            ///< Thème par défaut (vert néon)
        DARK,              ///< Thème sombre
        LIGHT,             ///< Thème clair
        MIDI_CONTROLLER,   ///< Thème optimisé pour contrôleur MIDI
        PERFORMANCE,       ///< Thème optimisé pour performances live
        STUDIO             ///< Thème pour usage studio
    };
    
    /**
     * @brief Configuration complète d'un thème
     */
    struct ThemeConfig {
        std::shared_ptr<UITheme> theme;      ///< Instance du thème
        std::shared_ptr<UILayout> layout;    ///< Layout associé
        String name;                         ///< Nom du thème
        String description;                  ///< Description
    };
    
    //=========================================================================
    // INTERFACE PUBLIQUE
    //=========================================================================
    
    /**
     * @brief Crée un thème complet avec layout
     * @param type Type de thème souhaité
     * @return Configuration complète du thème
     */
    static ThemeConfig createTheme(ThemeType type);
    
    /**
     * @brief Crée le thème par défaut du contrôleur MIDI
     * @return Configuration du thème par défaut
     */
    static ThemeConfig createMidiControllerTheme();
    
    /**
     * @brief Crée un thème optimisé pour les performances live
     * @return Configuration du thème performance
     */
    static ThemeConfig createPerformanceTheme();
    
    /**
     * @brief Crée un thème pour usage studio
     * @return Configuration du thème studio
     */
    static ThemeConfig createStudioTheme();
    
    /**
     * @brief Applique un thème à un display LVGL
     * @param config Configuration du thème
     * @param display Display LVGL cible
     */
    static void applyThemeToDisplay(const ThemeConfig& config, lv_display_t* display);
    
    /**
     * @brief Obtient la liste des thèmes disponibles
     * @return Liste des types de thèmes
     */
    static std::vector<ThemeType> getAvailableThemes();
    
    /**
     * @brief Obtient le nom d'un type de thème
     * @param type Type de thème
     * @return Nom du thème
     */
    static String getThemeName(ThemeType type);
    
    /**
     * @brief Obtient la description d'un type de thème
     * @param type Type de thème
     * @return Description du thème
     */
    static String getThemeDescription(ThemeType type);
    
private:
    //=========================================================================
    // MÉTHODES PRIVÉES DE CRÉATION
    //=========================================================================
    
    /**
     * @brief Crée le thème par défaut
     */
    static ThemeConfig createDefaultTheme();
    
    /**
     * @brief Crée le thème sombre
     */
    static ThemeConfig createDarkTheme();
    
    /**
     * @brief Crée le thème clair
     */
    static ThemeConfig createLightTheme();
    
    /**
     * @brief Configure le layout pour un thème donné
     * @param theme Thème à associer
     * @return Layout configuré
     */
    static std::shared_ptr<UILayout> createLayoutForTheme(std::shared_ptr<UITheme> theme);
    
    /**
     * @brief Applique des optimisations spécifiques à un thème
     * @param theme Thème à optimiser
     * @param type Type de thème
     */
    static void applyThemeOptimizations(UITheme& theme, ThemeType type);
};