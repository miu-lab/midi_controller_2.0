#pragma once

#include <memory>
#include <vector>
#include <Arduino.h>

#include "View.hpp"
#include "core/ports/output/DisplayPort.hpp"

/**
 * @brief Vue pour l'affichage et la surveillance des contrôles MIDI
 */
class ControlMonitorView : public View {
public:
    /**
     * @brief Structure représentant une information de contrôle MIDI
     */
    struct ControlInfo {
        uint8_t controlId;
        String type;
        uint8_t channel;
        uint8_t number;
        uint8_t value;
        unsigned long timestamp;
        
        ControlInfo(uint8_t id, const String& t, uint8_t ch, uint8_t num, uint8_t val)
            : controlId(id), type(t), channel(ch), number(num), value(val), 
              timestamp(millis()) {}
    };
    
    /**
     * @brief Constructeur
     * @param display Pointeur vers l'affichage
     */
    explicit ControlMonitorView(std::shared_ptr<DisplayPort> display);
    
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
     * @brief Met à jour les informations d'un contrôle
     * @param controlId ID du contrôle
     * @param type Type de message MIDI
     * @param channel Canal MIDI
     * @param number Numéro de contrôle ou note
     * @param value Valeur
     */
    void updateControlInfo(uint8_t controlId, const String& type, 
                          uint8_t channel, uint8_t number, uint8_t value);
    
    /**
     * @brief Met à jour la position d'un encodeur
     * @param encoderId ID de l'encodeur
     * @param position Position actuelle
     */
    void updateEncoderPosition(uint8_t encoderId, int32_t position);
    
    /**
     * @brief Met à jour l'état du bouton d'un encodeur
     * @param encoderId ID de l'encodeur
     * @param pressed État du bouton
     */
    void updateEncoderButtonState(uint8_t encoderId, bool pressed);
    
    /**
     * @brief Met à jour l'état d'un bouton
     * @param buttonId ID du bouton
     * @param pressed État du bouton
     */
    void updateButtonState(uint8_t buttonId, bool pressed);
    
    /**
     * @brief Fait défiler la vue
     * @param delta Quantité de défilement
     */
    void scroll(int8_t delta);

private:
    std::vector<ControlInfo> controlInfos_;
    std::vector<std::pair<uint8_t, int32_t>> encoderPositions_;
    std::vector<std::pair<uint8_t, bool>> buttonStates_;
    
    int selectedControlIndex_;
    int scrollOffset_;
    int maxVisibleControls_;
    
    unsigned long lastUpdateTime_;
};
