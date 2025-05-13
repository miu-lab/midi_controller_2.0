#pragma once

#include <Arduino.h>
#include <memory>

#include "View.hpp"
#include "core/ports/output/DisplayPort.hpp"

/**
 * @brief Vue pour l'écran de démarrage (splash screen)
 */
class SplashScreenView : public View {
public:
    /**
     * @brief Constructeur
     * @param display Pointeur vers l'affichage
     */
    explicit SplashScreenView(std::shared_ptr<DisplayPort> display);
    
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
     * @brief Vérifie si l'écran de démarrage doit être terminé
     * @return true si la durée du splash screen est écoulée
     */
    bool isSplashScreenCompleted() const;

private:
    String title_;
    String subtitle_;
    String version_;
    
    unsigned long startTime_;
    unsigned long duration_; // Durée d'affichage en ms
};
