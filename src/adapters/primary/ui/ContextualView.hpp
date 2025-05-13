#pragma once

#include <memory>
#include <Arduino.h>

#include "View.hpp"
#include "core/ports/output/DisplayPort.hpp"

/**
 * @brief Vue contextuelle affichant les informations principales
 * 
 * Cette vue affiche les informations contextuelles actuelles,
 * comme l'état des contrôles, les mappings actifs, etc.
 */
class ContextualView : public View {
public:
    /**
     * @brief Constructeur
     * @param display Pointeur vers l'affichage
     */
    explicit ContextualView(std::shared_ptr<DisplayPort> display);
    
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
     * @brief Définit le titre de la vue
     * @param title Titre à afficher
     */
    void setTitle(const String& title);
    
    /**
     * @brief Définit le message principal
     * @param message Message à afficher
     */
    void setMessage(const String& message);
    
    /**
     * @brief Fait défiler la vue
     * @param delta Quantité de défilement
     */
    void scroll(int delta);
    
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
     * @brief Définit le profil actif
     * @param profileName Nom du profil
     */
    void setActiveProfile(const String& profileName);

private:
    String title_;
    String message_;
    String activeProfile_;
    int scrollOffset_;
    unsigned long lastUpdateTime_;
    
    // Stockage des états des encodeurs et boutons
    static const int MAX_ENCODERS = 8;
    static const int MAX_BUTTONS = 16;
    
    struct EncoderState {
        int32_t position;
        bool buttonPressed;
        
        EncoderState() : position(0), buttonPressed(false) {}
    };
    
    EncoderState encoders_[MAX_ENCODERS];
    bool buttonStates_[MAX_BUTTONS];
};
