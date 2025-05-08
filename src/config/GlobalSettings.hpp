#pragma once

/**
 * @brief Configuration des paramètres globaux du système
 *
 * Cette classe contient tous les paramètres globaux qui peuvent être
 * modifiés pendant l'exécution.
 */
class GlobalSettings {
public:
    /**
     * @brief Obtient l'instance singleton des paramètres globaux
     * @return Référence à l'instance singleton
     */
    static GlobalSettings& getInstance() {
        static GlobalSettings instance;
        return instance;
    }

    /**
     * @brief Définit le facteur de sensibilité global pour les encodeurs
     * @param value Nouveau facteur de sensibilité (1.0 = normal, >1.0 = plus sensible, <1.0 = moins
     * sensible)
     */
    void setEncoderSensitivity(float value) {
        encoderSensitivity_ = value;
    }

    /**
     * @brief Obtient le facteur de sensibilité global pour les encodeurs
     * @return Facteur de sensibilité actuel
     */
    float getEncoderSensitivity() const {
        return encoderSensitivity_;
    }

private:
    // Constructeur privé (pattern Singleton)
    GlobalSettings() : encoderSensitivity_(1.0f) {}

    // Empêcher la copie
    GlobalSettings(const GlobalSettings&) = delete;
    GlobalSettings& operator=(const GlobalSettings&) = delete;

    // Paramètres
    float encoderSensitivity_;  // Facteur de sensibilité pour les encodeurs (1.0 = normal)
};
