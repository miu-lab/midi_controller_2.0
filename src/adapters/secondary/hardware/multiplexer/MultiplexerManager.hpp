#pragma once

#include <CD74HC4067.h>
#include <memory>
#include "config/SystemConstants.hpp"
#include "core/domain/types.hpp"

/**
 * @brief Gestionnaire singleton pour le multiplexeur CD74HC4067
 *
 * Gère la communication avec le multiplexeur 16 canaux
 * permettant d'étendre les entrées disponibles du Teensy.
 */
class MultiplexerManager {
public:
    /**
     * @brief Obtient l'instance unique du gestionnaire
     */
    static MultiplexerManager& getInstance();

    /**
     * @brief Initialise le multiplexeur avec les pins définies dans SystemConstants
     */
    void initialize();

    /**
     * @brief Sélectionne un canal du multiplexeur
     * @param channel Canal à sélectionner (0-15)
     */
    void selectChannel(uint8_t channel);

    /**
     * @brief Lit une valeur digitale du canal actuellement sélectionné
     * @return État logique du canal (HIGH/LOW)
     */
    bool readDigital() const;

    /**
     * @brief Lit une valeur analogique du canal actuellement sélectionné
     * @return Valeur analogique (0-1023 sur Teensy)
     */
    uint16_t readAnalog() const;

    /**
     * @brief Lit directement depuis un canal spécifique
     * @param channel Canal à lire (0-15)
     * @return État logique du canal
     */
    bool readDigitalFromChannel(uint8_t channel);

    /**
     * @brief Lit une valeur analogique depuis un canal spécifique
     * @param channel Canal à lire (0-15)
     * @return Valeur analogique
     */
    uint16_t readAnalogFromChannel(uint8_t channel);

    /**
     * @brief Vérifie si le multiplexeur est initialisé
     */
    bool isInitialized() const { return initialized_; }

    /**
     * @brief Obtient le canal actuellement sélectionné
     */
    uint8_t getCurrentChannel() const { return currentChannel_; }

private:
    MultiplexerManager();
    ~MultiplexerManager() = default;

    // Empêche la copie
    MultiplexerManager(const MultiplexerManager&) = delete;
    MultiplexerManager& operator=(const MultiplexerManager&) = delete;

    std::unique_ptr<CD74HC4067> mux_;
    uint8_t currentChannel_ = 0;
    bool initialized_ = false;
};