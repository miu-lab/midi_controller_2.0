#pragma once

#include <functional>
#include <memory>

#include "app/services/NavigationConfigService.hpp"
#include "core/domain/events/EventTypes.hpp"
#include "core/domain/types.hpp"

// Forward declaration
class UIController;

/**
 * @brief Contrôleur dédié à la gestion des entrées physiques
 *
 * Ce contrôleur est responsable du routage des entrées physiques (encodeurs, boutons)
 * vers les contrôleurs appropriés (navigation ou MIDI) en fonction de la configuration.
 */
class InputController {
public:
    /**
     * @brief Constructeur
     * @param navigationConfig Service de configuration des contrôles de navigation
     */
    InputController(NavigationConfigService& navigationConfig);

    /**
     * @brief Configure le contrôleur d'interface utilisateur à utiliser
     * @param uiController Contrôleur d'interface utilisateur
     */
    void setUIController(UIController* uiController);

    /**
     * @brief Traite la rotation d'un encodeur
     * @param id Identifiant de l'encodeur
     * @param absolutePosition Position absolue de l'encodeur
     * @param relativeChange Changement relatif de position
     */
    void processEncoderTurn(EncoderId id, int32_t absolutePosition, int8_t relativeChange);

    /**
     * @brief Traite l'appui sur le bouton d'un encodeur
     * @param id Identifiant de l'encodeur
     * @param pressed État du bouton (true = pressé, false = relâché)
     */
    void processEncoderButton(EncoderId id, bool pressed);

    /**
     * @brief Traite l'appui sur un bouton
     * @param id Identifiant du bouton
     * @param pressed État du bouton (true = pressé, false = relâché)
     */
    void processButtonPress(ButtonId id, bool pressed);

    /**
     * @brief Définit le callback pour les encodeurs de navigation
     * @param callback Fonction à appeler lors de la rotation d'un encodeur de navigation
     */
    void setNavigationEncoderCallback(std::function<void(EncoderId, int32_t, int8_t)> callback);

    /**
     * @brief Définit le callback pour les encodeurs MIDI
     * @param callback Fonction à appeler lors de la rotation d'un encodeur MIDI
     */
    void setMidiEncoderCallback(std::function<void(EncoderId, int32_t, int8_t)> callback);

    /**
     * @brief Définit le callback pour les boutons d'encodeur de navigation
     * @param callback Fonction à appeler lors de l'appui sur un bouton d'encodeur de navigation
     */
    void setNavigationEncoderButtonCallback(std::function<void(EncoderId, bool)> callback);

    /**
     * @brief Définit le callback pour les boutons d'encodeur MIDI
     * @param callback Fonction à appeler lors de l'appui sur un bouton d'encodeur MIDI
     */
    void setMidiEncoderButtonCallback(std::function<void(EncoderId, bool)> callback);

    /**
     * @brief Définit le callback pour les boutons de navigation
     * @param callback Fonction à appeler lors de l'appui sur un bouton de navigation
     */
    void setNavigationButtonCallback(std::function<void(ButtonId, bool)> callback);

    /**
     * @brief Définit le callback pour les boutons MIDI
     * @param callback Fonction à appeler lors de l'appui sur un bouton MIDI
     */
    void setMidiButtonCallback(std::function<void(ButtonId, bool)> callback);

private:
    NavigationConfigService& navigationConfig_;
    UIController* uiController_;

    // Stockage des callbacks
    std::function<void(EncoderId, int32_t, int8_t)> navigationEncoderCallback_;
    std::function<void(EncoderId, int32_t, int8_t)> midiEncoderCallback_;
    std::function<void(EncoderId, bool)> navigationEncoderButtonCallback_;
    std::function<void(EncoderId, bool)> midiEncoderButtonCallback_;
    std::function<void(ButtonId, bool)> navigationButtonCallback_;
    std::function<void(ButtonId, bool)> midiButtonCallback_;

    // Méthodes pour traiter les entrées via l'UIController
    void handleNavigationEncoderTurn(EncoderId id, int32_t absolutePosition, int8_t relativeChange);
    void handleNavigationEncoderButton(EncoderId id, bool pressed);
    void handleNavigationButton(ButtonId id, bool pressed);
};
