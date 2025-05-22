#pragma once

#include <Arduino.h>

#include <memory>

#include "View.hpp"
#include "core/ports/output/DisplayPort.hpp"

/**
 * @brief Vue focalisée sur un paramètre MIDI avec visualisation circulaire remplie
 *
 * Cette vue affiche un paramètre MIDI avec une interface immersive similaire
 * à l'image de référence :
 * - CC numéro en haut à gauche (ex: "CC 127")
 * - Canal MIDI en haut à droite (ex: "CH 16")
 * - Grand cercle central avec remplissage proportionnel à la valeur
 * - Nom du paramètre en bas centre (ex: "CUTOFF")
 *
 * Design inspiré de l'interface de cutoff présentée dans l'image.
 */
class ParameterFocusView : public View {
public:
    /**
     * @brief Constructeur
     * @param display Pointeur vers l'affichage
     */
    explicit ParameterFocusView(std::shared_ptr<DisplayPort> display);

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
     * @brief Met à jour les informations du paramètre
     * @param ccNumber Numéro de Control Change (0-127)
     * @param channel Canal MIDI (1-16)
     * @param value Valeur du paramètre (0-127)
     * @param parameterName Nom du paramètre (ex: "CUTOFF", "RESONANCE")
     */
    void setParameter(uint8_t ccNumber, uint8_t channel, uint8_t value,
                      const String& parameterName);

    /**
     * @brief Met à jour uniquement la valeur (pour les changements rapides)
     * @param value Nouvelle valeur (0-127)
     */
    void setValue(uint8_t value);

    /**
     * @brief Met à jour uniquement le nom du paramètre
     * @param parameterName Nouveau nom du paramètre
     */
    void setParameterName(const String& parameterName);

private:
    // Informations sur le paramètre MIDI
    uint8_t ccNumber_;      // Numéro CC (0-127)
    uint8_t channel_;       // Canal MIDI (1-16)
    uint8_t value_;         // Valeur (0-127)
    String parameterName_;  // Nom du paramètre

    // État de l'affichage
    bool needsRedraw_;
    unsigned long lastUpdateTime_;

    // Configuration du layout
    static const int CC_LABEL_X = 2;
    static const int CC_LABEL_Y = 2;
    static const int CH_LABEL_X = 101;
    static const int CH_LABEL_Y = 2;
    static const int PARAM_NAME_Y = 56;

    // Configuration du cercle principal
    static const int CIRCLE_CENTER_X = 64;
    static const int CIRCLE_CENTER_Y = 32;
    static const int CIRCLE_RADIUS = 20;
    static const int CIRCLE_THICKNESS = 2;

    // Méthodes privées pour le rendu
    void drawLabels();
    void drawParameterCircle();
    void drawParameterName();

    /**
     * @brief Dessine un cercle rempli avec un arc proportionnel à la valeur
     * @param centerX Centre X du cercle
     * @param centerY Centre Y du cercle
     * @param radius Rayon du cercle
     * @param fillRatio Ratio de remplissage (0.0 à 1.0)
     */
    void drawFilledCircleArc(int centerX, int centerY, int radius, float fillRatio);

    /**
     * @brief Calcule le ratio de remplissage basé sur la valeur MIDI
     * @return Ratio entre 0.0 et 1.0
     */
    float calculateFillRatio() const;
};