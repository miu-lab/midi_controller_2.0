#pragma once

#include "core/domain/events/EventSystem.hpp"

// Forward declaration
class UIController;

/**
 * @brief Écouteur d'événements pour le contrôleur UI
 */
class UIEventListener : public EventListener {
public:
    /**
     * @brief Constructeur
     * @param uiController Contrôleur UI à notifier
     */
    UIEventListener(UIController& uiController);
    
    /**
     * @brief Initialise les abonnements aux événements
     */
    void initialize();
    
    /**
     * @brief Nettoie les abonnements aux événements
     */
    void cleanup();
    
    /**
     * @brief Méthode appelée lors de la réception d'un événement
     * @param event Événement reçu
     * @return true si l'événement a été traité, false sinon
     */
    virtual bool onEvent(const Event& event) override;
    
private:
    /**
     * @brief Gère les événements d'encodeur tourné
     */
    bool handleEncoderTurn(const EncoderTurnedEvent& event);
    
    /**
     * @brief Gère les événements de bouton d'encodeur
     */
    bool handleEncoderButton(const EncoderButtonEvent& event);
    
    /**
     * @brief Gère les événements de bouton pressé
     */
    bool handleButtonPressed(const ButtonPressedEvent& event);
    
    /**
     * @brief Gère les événements de bouton relâché
     */
    bool handleButtonReleased(const ButtonReleasedEvent& event);
    
    UIController& uiController_;   // Référence au contrôleur UI
    SubscriptionId subscriptionId_;  // ID d'abonnement
    
    // Identifiants des encodeurs et boutons de navigation
    static constexpr uint8_t NAVIGATION_ENCODER = 0;  // Encodeur de navigation
    static constexpr uint8_t BACK_BUTTON = 0;         // Bouton retour
    static constexpr uint8_t OK_BUTTON = 1;           // Bouton OK
};
