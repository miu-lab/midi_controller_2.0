#pragma once
#include "interface_adapters/encoders/EncoderConfig.hpp"
#include "interface_adapters/buttons/DigitalButtonConfig.hpp"
#include "domain/types.hpp"
#include "app/NavigationConfigService.hpp"
#include "config/NavigationMappingConfig.hpp"
#include "storage/ProfileManager.hpp"
#include <vector>
#include <cstddef>

/**
 * @brief Service centralisant la gestion des configurations
 * 
 * Ce service est responsable du chargement et de la gestion de toutes
 * les configurations de l'application : encodeurs, boutons, mappings MIDI,
 * contrôles de navigation, etc.
 */
class ConfigurationService {
public:
    /**
     * @brief Construit un nouveau service de configuration
     */
    ConfigurationService();
    
    /**
     * @brief Charge toutes les configurations par défaut
     */
    void loadDefaultConfigurations();
    
    /**
     * @brief Applique les configurations aux différents services
     * @param profileManager Gestionnaire de profils pour les mappings MIDI
     * @param navService Service de configuration pour la navigation
     */
    void applyConfigurations(ProfileManager& profileManager, 
                             NavigationConfigService& navService);
    
    /**
     * @brief Accès à la configuration des encodeurs
     * @return Référence constante au vecteur de configurations d'encodeurs
     */
    const std::vector<EncoderConfig>& getEncoderConfigs() const;
    
    /**
     * @brief Accès à la configuration des boutons
     * @return Référence constante au vecteur de configurations de boutons
     */
    const std::vector<ButtonConfig>& getButtonConfigs() const;
    
private:
    // Stockage des configurations
    std::vector<EncoderConfig> encoderConfigs_;
    std::vector<ButtonConfig> buttonConfigs_;
    std::vector<MidiControlMapping> midiMappings_;
    std::vector<NavigationControl> navigationControls_;
    
    // Méthodes privées de chargement
    void loadEncoderConfigs();
    void loadButtonConfigs();
    void loadMidiMappings();
    void loadNavigationControls();
};
