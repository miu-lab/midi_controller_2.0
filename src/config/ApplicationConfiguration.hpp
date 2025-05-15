#pragma once

#include <functional>
#include <string>
#include <vector>

#include "config/ConfigDefaults.hpp"
#include "config/HardwareConfiguration.hpp"
#include "config/MappingConfiguration.hpp"

/**
 * @brief Configuration centrale de l'application
 *
 * Cette classe sert de point d'entrée unique pour toutes les configurations
 * de l'application. Elle regroupe les configurations spécifiques et
 * les paramètres globaux organisés par catégorie.
 */
class ApplicationConfiguration {
public:
    /**
     * @brief Modes opérationnels prédéfinis
     */
    enum class OperationMode {
        NORMAL,       // Mode standard avec toutes les fonctionnalités
        PERFORMANCE,  // Mode optimisé pour les performances (réduit les diagnostics)
        DEBUG_MODE  // Mode avec informations de débogage étendues (renommé pour éviter le conflit
                    // avec la macro DEBUG)
    };

    /**
     * @brief Configuration liée aux performances
     */
    struct PerformanceSettings {
        bool performanceMode;
        unsigned long maxUpdateCycleTimeUs;
        unsigned long maxInputTimeUs;
        unsigned long maxMidiTimeUs;
        unsigned long maxUiTimeUs;
    };

    /**
     * @brief Configuration de l'interface utilisateur
     */
    struct UISettings {
        int refreshRateHz;
        unsigned long refreshPeriodMs;
        bool showDebugInfo;
        bool enableFullUI;
    };

    /**
     * @brief Configuration des contrôles
     */
    struct ControlSettings {
        float encoderSensitivity;
        unsigned long encoderRateLimitMs;
    };

    /**
     * @brief Configuration MIDI
     */
    struct MidiSettings {
        uint8_t defaultChannel;
        uint8_t ccValueMin;
        uint8_t ccValueMax;
    };

    /**
     * @brief Configuration système
     */
    struct SystemSettings {
        unsigned long serialBaudRate;
        size_t maxCommandHistory;
    };

    /**
     * @brief Type de fonction callback pour les notifications de changement
     */
    using ChangeCallback = std::function<void(const std::string& paramName)>;

    /**
     * @brief Constructeur par défaut
     */
    ApplicationConfiguration();

    /**
     * @brief Obtient la configuration matérielle
     * @return Une référence à la configuration matérielle
     */
    const HardwareConfiguration& getHardwareConfiguration() const;

    /**
     * @brief Obtient la configuration des mappings
     * @return Une référence à la configuration des mappings
     */
    const MappingConfiguration& getMappingConfiguration() const;

    /**
     * @brief Obtient les paramètres de performance
     * @return Une référence aux paramètres de performance
     */
    const PerformanceSettings& getPerformanceSettings() const;
    void setPerformanceSettings(const PerformanceSettings& settings);

    /**
     * @brief Obtient les paramètres de l'interface utilisateur
     * @return Une référence aux paramètres de l'interface utilisateur
     */
    const UISettings& getUISettings() const;
    void setUISettings(const UISettings& settings);

    /**
     * @brief Obtient les paramètres des contrôles
     * @return Une référence aux paramètres des contrôles
     */
    const ControlSettings& getControlSettings() const;
    void setControlSettings(const ControlSettings& settings);

    /**
     * @brief Obtient les paramètres MIDI
     * @return Une référence aux paramètres MIDI
     */
    const MidiSettings& getMidiSettings() const;
    void setMidiSettings(const MidiSettings& settings);

    /**
     * @brief Obtient les paramètres système
     * @return Une référence aux paramètres système
     */
    const SystemSettings& getSystemSettings() const;
    void setSystemSettings(const SystemSettings& settings);

    /**
     * @brief Méthode de compatibilité pour la sensibilité des encodeurs
     * @param value Nouveau facteur de sensibilité
     */
    void setEncoderSensitivity(float value);
    float getEncoderSensitivity() const;

    /**
     * @brief Applique les paramètres selon le mode d'opération
     * @param mode Mode d'opération à appliquer
     */
    void setOperationMode(OperationMode mode);

    /**
     * @brief Charge la configuration depuis le stockage
     * @return true si le chargement a réussi, false sinon
     */
    bool loadFromStorage();

    /**
     * @brief Sauvegarde la configuration dans le stockage
     * @return true si la sauvegarde a réussi, false sinon
     */
    bool saveToStorage() const;

    /**
     * @brief Réinitialise tous les paramètres aux valeurs par défaut
     */
    void resetToDefaults();

    /**
     * @brief Enregistre un callback de notification de changement
     * @param callback Fonction à appeler lors d'un changement
     */
    void registerChangeCallback(ChangeCallback callback);

    /**
     * @brief Supprime un callback de notification
     * @param callback Fonction à supprimer
     */
    void unregisterChangeCallback(const ChangeCallback& callback);

private:
    HardwareConfiguration hardwareConfiguration;
    MappingConfiguration mappingConfiguration;

    PerformanceSettings performanceSettings;
    UISettings uiSettings;
    ControlSettings controlSettings;
    MidiSettings midiSettings;
    SystemSettings systemSettings;

    std::vector<ChangeCallback> changeCallbacks;

    // Méthode utilitaire pour notifier les changements
    void notifyChange(const std::string& paramName);
};
