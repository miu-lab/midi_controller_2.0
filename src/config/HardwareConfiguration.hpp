#pragma once

#include <vector>
#include <optional>

#include "adapters/secondary/hardware/input/InputConfig.hpp"

/**
 * @brief Configuration matérielle des boutons et encodeurs
 *
 * Cette classe centralise toutes les configurations matérielles du contrôleur MIDI.
 * Interface unifiée basée sur InputConfig.
 */
class HardwareConfiguration {
public:
    HardwareConfiguration();

    /**
     * @brief Obtient toutes les configurations d'entrée unifiées
     * @return Un vecteur constant de configurations d'entrée
     */
    const std::vector<InputConfig>& getAllInputConfigurations() const;

    // === MÉTHODES UTILITAIRES ===
    
    /**
     * @brief Filtre les configurations par type
     * @param type Type d'entrée à filtrer (BUTTON ou ENCODER)
     * @return Vecteur des configurations du type demandé
     */
    std::vector<InputConfig> getInputConfigurationsByType(InputType type) const;
    
    /**
     * @brief Obtient une configuration par ID
     * @param id Identifiant de l'entrée
     * @return Configuration trouvée ou std::nullopt
     */
    std::optional<InputConfig> getInputConfigurationById(InputId id) const;
    
    /**
     * @brief Valide toutes les configurations
     * @return true si toutes les configurations sont valides
     */
    bool validateAllConfigurations() const;

private:
    std::vector<InputConfig> inputConfigurations_;  ///< Configurations unifiées

    // Méthodes d'initialisation
    void setInputsConfiguration();
    
    // Helpers pour la création
    InputConfig createButtonInput(InputId id, const std::string& name, const std::string& label, 
                                 const ButtonConfig& buttonConfig, const std::string& group = "Control");
    InputConfig createEncoderInput(InputId id, const std::string& name, const std::string& label, 
                                  const EncoderConfig& encoderConfig, const std::string& group = "MIDI");
};
