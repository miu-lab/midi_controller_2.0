#pragma once

/**
 * @brief Interface pour la gestion d'affichage avec timing et rafraîchissement optimisé
 * 
 * Cette interface définit les opérations pour gérer les rafraîchissements d'écran
 * avec limitation de fréquence et timing optimal pour les performances.
 */
class IDisplayManager {
public:
    /**
     * @brief Destructeur virtuel
     */
    virtual ~IDisplayManager() = default;
    
    /**
     * @brief Met à jour l'affichage si nécessaire
     * 
     * Vérifie le timing et rafraîchit l'écran uniquement si l'intervalle
     * requis s'est écoulé pour optimiser les performances.
     */
    virtual void update() = 0;
    
    /**
     * @brief Force un rafraîchissement immédiat de l'affichage
     */
    virtual void forceRefresh() = 0;
    
    /**
     * @brief Configure l'intervalle de rafraîchissement
     * @param intervalMs Intervalle en millisecondes
     */
    virtual void setRefreshInterval(unsigned long intervalMs) = 0;
    
    /**
     * @brief Obtient l'intervalle de rafraîchissement actuel
     * @return Intervalle en millisecondes
     */
    virtual unsigned long getRefreshInterval() const = 0;
};