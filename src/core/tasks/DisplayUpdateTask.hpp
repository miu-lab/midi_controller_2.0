#pragma once
#include <memory>

#include "core/TaskScheduler.hpp"
#include "core/ports/output/DisplayPort.hpp"

/**
 * @brief Tâche dédiée à la mise à jour de l'affichage
 *
 * Cette tâche gère la mise à jour de l'affichage de manière non bloquante,
 * permettant de limiter la fréquence des updates et d'éviter de bloquer
 * le thread principal.
 */
class DisplayUpdateTask {
public:
    /**
     * @brief Constructeur
     * @param display Référence à l'adaptateur d'affichage
     * @param intervalMs Intervalle minimum entre les mises à jour en ms (limite le framerate)
     */
    DisplayUpdateTask(std::shared_ptr<DisplayPort> display, uint32_t intervalMs = 50)
        : display_(display), needsUpdate_(false), intervalMs_(intervalMs) {}

    /**
     * @brief Exécute la tâche si une mise à jour est nécessaire
     */
    void execute() {
        // Pour le débogage, ajouter un log périodique pour vérifier que la tâche s'exécute
        static uint32_t taskCallCount = 0;
        if (++taskCallCount % 100 == 0) {  // Log toutes les 100 exécutions environ
            Serial.printf("DisplayUpdateTask: Called %lu times, needs update: %s\n",
                          taskCallCount,
                          needsUpdate_ ? "yes" : "no");
        }

        if (needsUpdate_) {
// Mesurer le temps d'affichage en mode debug
#ifdef DEBUG_DISPLAY_PERFORMANCE
            unsigned long startTime = micros();
#endif

            display_->update();
            needsUpdate_ = false;

#ifdef DEBUG_DISPLAY_PERFORMANCE
            unsigned long endTime = micros();
            Serial.printf("Display update took: %lu us\n", endTime - startTime);
#endif
        }
    }

    /**
     * @brief Demande une mise à jour de l'affichage
     */
    void requestUpdate() {
        needsUpdate_ = true;
    }

    /**
     * @brief Obtenir la fonction à exécuter pour cette tâche
     * @return Fonction à exécuter
     */
    TaskFunction getTaskFunction() {
        return [this]() { this->execute(); };
    }

    /**
     * @brief Obtenir l'intervalle d'exécution
     * @return Intervalle en microsecondes
     */
    uint32_t getIntervalMicros() const {
        return intervalMs_ * 1000;  // Conversion ms -> μs
    }

    /**
     * @brief Obtenir la priorité de la tâche
     * @return Priorité (0 = plus haute)
     */
    uint8_t getPriority() const {
        return 1;  // Priorité moyenne-haute
    }

    /**
     * @brief Obtenir le nom de la tâche pour le débogage
     * @return Nom de la tâche
     */
    const char* getName() const {
        return "DisplayUpdate";
    }

private:
    std::shared_ptr<DisplayPort> display_;
    bool needsUpdate_;
    uint32_t intervalMs_;
};
