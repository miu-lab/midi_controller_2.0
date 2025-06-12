#pragma once
#include <Arduino.h>
#include <climits> // Pour ULONG_MAX

/**
 * @brief Classe utilitaire pour mesurer les performances d'affichage
 * 
 * Permet de mesurer le temps pris par les opérations d'affichage,
 * ce qui est utile pour optimiser les performances de l'interface
 * utilisateur.
 */
class DisplayProfiler {
public:
    /**
     * @brief Enregistre directement une durée de mise à jour
     * @param duration Durée en microsecondes
     */
    void recordUpdate(unsigned long duration) {
        updateHistory(duration);
    }
    
    /**
     * @brief Commence une mesure de temps
     */
    void startMeasurement() {
        startTime_ = micros();
    }
    
    /**
     * @brief Termine une mesure et enregistre la durée
     * @return Durée mesurée en microsecondes
     */
    unsigned long endMeasurement() {
        unsigned long duration = micros() - startTime_;
        updateHistory(duration);
        return duration;
    }
    
    /**
     * @brief Obtient le temps moyen des mises à jour
     * @return Temps moyen en microsecondes
     */
    unsigned long getAverageUpdateTime() const {
        return totalUpdates_ > 0 ? totalTime_ / totalUpdates_ : 0;
    }
    
    /**
     * @brief Obtient le temps maximum des mises à jour
     * @return Temps maximum en microsecondes
     */
    unsigned long getMaxUpdateTime() const {
        return maxTime_;
    }
    
    /**
     * @brief Obtient le temps minimum des mises à jour
     * @return Temps minimum en microsecondes
     */
    unsigned long getMinUpdateTime() const {
        return minTime_ == ULONG_MAX ? 0 : minTime_;
    }
    
    /**
     * @brief Réinitialise les compteurs
     */
    void reset() {
        totalUpdates_ = 0;
        totalTime_ = 0;
        maxTime_ = 0;
        minTime_ = ULONG_MAX;
    }

private:
    unsigned long startTime_ = 0;
    unsigned long totalUpdates_ = 0;
    unsigned long totalTime_ = 0;
    unsigned long maxTime_ = 0;
    unsigned long minTime_ = ULONG_MAX;
    
    /**
     * @brief Met à jour les statistiques d'historique
     * @param duration Durée à enregistrer
     */
    void updateHistory(unsigned long duration) {
        totalUpdates_++;
        totalTime_ += duration;
        maxTime_ = max(maxTime_, duration);
        minTime_ = min(minTime_, duration);
    }
};
