#pragma once

#include <Arduino.h>
#include <memory>

#include "View.hpp"
#include "core/ports/output/DisplayPort.hpp"

/**
 * @brief Vue qui affiche les informations du dernier contrôle MIDI manipulé
 */
class LastControlView : public View {
public:
    /**
     * @brief Constructeur
     * @param display Pointeur vers l'affichage
     */
    explicit LastControlView(std::shared_ptr<DisplayPort> display);
    
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
     * @brief Met à jour les informations sur le dernier contrôle manipulé
     * @param controlId ID du contrôle
     * @param type Type de message MIDI ("CC", "Note On", etc.)
     * @param channel Canal MIDI
     * @param number Numéro de CC ou note
     * @param value Valeur
     */
    void updateLastControl(uint8_t controlId, const String& type, 
                         uint8_t channel, uint8_t number, uint8_t value);

private:
    String title_;
    
    // Informations sur le dernier contrôle
    uint8_t lastControlId_;
    String lastControlType_;
    uint8_t lastChannel_;
    uint8_t lastNumber_;
    uint8_t lastValue_;
    
    // Horodatage de la dernière mise à jour
    unsigned long lastUpdateTime_;
    
    // Animation
    bool animationActive_;
    unsigned long animationStartTime_;
    static const unsigned long ANIMATION_DURATION = 300; // ms
};
