#pragma once

#include <Arduino.h>
#include "app/services/InputSystem.hpp"
#include "core/domain/events/EventSystem.hpp"
#include "adapters/secondary/hardware/encoders/EncoderManager.hpp"
#include "adapters/secondary/hardware/buttons/DigitalButtonManager.hpp"
#include "core/use_cases/ProcessButtons.hpp"
#include "core/use_cases/ProcessEncoders.hpp"

/**
 * @brief Système d'entrée utilisant le bus d'événements
 * Note: Hérite d'InputSystem pour la compatibilité avec le ServiceLocator
 */
class EventInputSystem : public InputSystem {
public:
    /**
     * @brief Constructeur par défaut
     */
    EventInputSystem();
    
    /**
     * @brief Initialisation du système
     */
    void init();
    
    /**
     * @brief Mise à jour périodique
     * Doit être appelée régulièrement pour traiter les entrées
     */
    void update();
    
    /**
     * @brief Initialisation avec configurations spécifiques
     * 
     * @param encoderConfigs Liste de configurations d'encodeurs
     * @param buttonConfigs Liste de configurations de boutons
     */
    void init(const std::vector<EncoderConfig>& encoderConfigs,
              const std::vector<ButtonConfig>& buttonConfigs);
              
private:
    /**
     * @brief Callback pour les rotations d'encodeurs
     * 
     * @param id Identifiant de l'encodeur
     * @param position Position absolue
     * @param delta Changement relatif
     */
    void handleEncoderTurn(uint8_t id, int32_t position, int8_t delta);
    
    /**
     * @brief Callback pour les boutons d'encodeurs
     * 
     * @param id Identifiant de l'encodeur
     * @param pressed État du bouton
     */
    void handleEncoderButton(uint8_t id, bool pressed);
    
    /**
     * @brief Callback pour les boutons
     * 
     * @param id Identifiant du bouton
     * @param pressed État du bouton
     */
    void handleButton(uint8_t id, bool pressed);
    
    // Gestionnaires matériels
    EncoderManager encoderManager_;
    ProcessEncoders processEncoders_;
    DigitalButtonManager buttonManager_;
    ProcessButtons processButtons_;
};
