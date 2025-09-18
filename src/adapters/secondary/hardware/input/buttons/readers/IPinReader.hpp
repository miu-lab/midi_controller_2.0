#pragma once

/**
 * @brief Interface Strategy pour la lecture de pins
 *
 * Cette interface permet d'abstraire la source de lecture des pins
 * (directe, multiplexée, I2C, etc.) et facilite les tests unitaires.
 */
class IPinReader {
public:
    virtual ~IPinReader() = default;

    /**
     * @brief Lit l'état brut de la pin
     * @return true si HIGH, false si LOW
     */
    virtual bool readPin() = 0;

    /**
     * @brief Initialise la configuration de la pin si nécessaire
     * Appelé une seule fois lors de la création du bouton
     */
    virtual void initialize() {}
};