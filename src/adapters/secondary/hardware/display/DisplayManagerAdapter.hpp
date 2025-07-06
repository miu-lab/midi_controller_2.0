#pragma once

#include <memory>
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "core/domain/interfaces/IDisplayManager.hpp"

/**
 * @brief Adapter pour la gestion d'affichage avec timing et rafraîchissement optimisé
 * 
 * Cette classe implémente IDisplayManager et est responsable de gérer
 * les rafraîchissements d'écran avec limitation de fréquence et timing optimal.
 */
class DisplayManagerAdapter : public IDisplayManager {
public:
    /**
     * @brief Constructeur avec bridge LVGL
     * @param lvglBridge Bridge pour l'affichage LVGL
     */
    explicit DisplayManagerAdapter(std::shared_ptr<Ili9341LvglBridge> lvglBridge);
    
    /**
     * @brief Destructeur par défaut
     */
    ~DisplayManagerAdapter() override = default;
    
    /**
     * @brief Met à jour l'affichage si nécessaire
     * 
     * Vérifie le timing et rafraîchit l'écran uniquement si l'intervalle
     * requis s'est écoulé pour optimiser les performances.
     */
    void update() override;
    
    /**
     * @brief Force un rafraîchissement immédiat de l'affichage
     */
    void forceRefresh() override;
    
    /**
     * @brief Configure l'intervalle de rafraîchissement
     * @param intervalMs Intervalle en millisecondes
     */
    void setRefreshInterval(unsigned long intervalMs) override;
    
    /**
     * @brief Obtient l'intervalle de rafraîchissement actuel
     * @return Intervalle en millisecondes
     */
    unsigned long getRefreshInterval() const override;

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