#pragma once

#include <memory>
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"

/**
 * @brief Gestionnaire d'affichage avec timing et rafraîchissement optimisé
 * 
 * Responsable de gérer les rafraîchissements d'écran avec limitation de fréquence
 * et timing optimal pour les performances.
 */
class DisplayManager {
public:
    /**
     * @brief Constructeur avec bridge LVGL
     * @param lvglBridge Bridge pour l'affichage LVGL
     */
    explicit DisplayManager(std::shared_ptr<Ili9341LvglBridge> lvglBridge);
    
    /**
     * @brief Destructeur par défaut
     */
    ~DisplayManager() = default;
    
    /**
     * @brief Met à jour l'affichage si nécessaire
     * 
     * Vérifie le timing et rafraîchit l'écran uniquement si l'intervalle
     * requis s'est écoulé pour optimiser les performances.
     */
    void update();
    
    /**
     * @brief Force un rafraîchissement immédiat de l'affichage
     */
    void forceRefresh();
    
    /**
     * @brief Configure l'intervalle de rafraîchissement
     * @param intervalMs Intervalle en millisecondes
     */
    void setRefreshInterval(unsigned long intervalMs);
    
    /**
     * @brief Obtient l'intervalle de rafraîchissement actuel
     * @return Intervalle en millisecondes
     */
    unsigned long getRefreshInterval() const;

private:
    std::shared_ptr<Ili9341LvglBridge> lvglBridge_;
    unsigned long refreshIntervalMs_;
    unsigned long lastRefreshTime_;
    
    /**
     * @brief Obtient le temps actuel en millisecondes
     * @return Temps actuel
     */
    unsigned long getCurrentTime() const;
};