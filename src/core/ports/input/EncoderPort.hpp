#pragma once

#include "core/domain/types.hpp"

/**
 * @brief Port d'entrée pour les encodeurs
 *
 * Cette interface définit le contrat pour l'interaction avec les encodeurs physiques.
 */
class EncoderPort {
public:
    /**
     * @brief Destructeur virtuel
     */
    virtual ~EncoderPort() = default;

    /**
     * @brief Lit le changement de position de l'encodeur
     * @return Différence de position depuis la dernière lecture
     */
    virtual int8_t readDelta() = 0;


    /**
     * @brief Obtient l'identifiant de l'encodeur
     * @return ID de l'encodeur
     */
    virtual EncoderId getId() const = 0;

    /**
     * @brief Obtient la résolution de l'encodeur (pulses par rotation)
     * @return Résolution en PPR
     */
    virtual uint16_t getPpr() const = 0;

    /**
     * @brief Obtient la position absolue de l'encodeur
     * @return Position absolue
     */
    virtual int32_t getAbsolutePosition() const = 0;

    /**
     * @brief Obtient la position physique brute de l'encodeur
     * @return Position physique
     */
    virtual int32_t getPhysicalPosition() const = 0;

    /**
     * @brief Réinitialise la position de l'encodeur
     */
    virtual void resetPosition() = 0;
};
