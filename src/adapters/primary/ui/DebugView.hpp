#pragma once

#include <memory>
#include <vector>
#include <Arduino.h>

#include "View.hpp"
#include "core/ports/output/DisplayPort.hpp"

/**
 * @brief Vue pour l'affichage des logs de débogage
 */
class DebugView : public View {
public:
    /**
     * @brief Constructeur
     * @param display Pointeur vers l'affichage
     */
    explicit DebugView(std::shared_ptr<DisplayPort> display);
    
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
     * @brief Ajoute une entrée de log
     * @param message Message de log
     */
    void addLogEntry(const String& message);
    
    /**
     * @brief Fait défiler les logs
     * @param delta Quantité de défilement
     */
    void scroll(int8_t delta);
    
    /**
     * @brief Efface tous les logs
     */
    void clearLogs();

private:
    std::vector<String> logEntries_;
    int scrollOffset_;
    int maxVisibleLines_;
    unsigned long lastUpdateTime_;
    bool autoScroll_;
};
