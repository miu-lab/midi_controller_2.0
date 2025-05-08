#pragma once
#include "adapters/secondary/hardware/encoders/EncoderManager.hpp"
#include "core/use_cases/ProcessEncoders.hpp"
#include "adapters/secondary/hardware/buttons/DigitalButtonManager.hpp"
#include "core/use_cases/ProcessButtons.hpp"
#include <vector>

/**
 * @brief Système gérant toutes les entrées physiques
 * 
 * Ce système regroupe les gestionnaires d'encodeurs et de boutons,
 * ainsi que le traitement des entrées en événements.
 */
class InputSystem {
public:
    /**
     * @brief Construit un nouveau système d'entrée
     */
    InputSystem();
    
    /**
     * @brief Initialise le système d'entrée
     */
    void init();
    
    /**
     * @brief Initialise le système d'entrée avec les configurations spécifiées
     * @param encoderConfigs Configuration des encodeurs
     * @param buttonConfigs Configuration des boutons
     */
    void init(const std::vector<EncoderConfig>& encoderConfigs, 
              const std::vector<ButtonConfig>& buttonConfigs);
    
    /**
     * @brief Met à jour le système d'entrée
     * 
     * Lit les états des encodeurs et des boutons et génère les événements correspondants.
     */
    void update();
    
private:
    EncoderManager       encoderManager_;   // Gestionnaire des encodeurs
    ProcessEncoders      processEncoders_;  // Traitement des encodeurs
    DigitalButtonManager buttonManager_;    // Gestionnaire des boutons
    ProcessButtons       processButtons_;   // Traitement des boutons
};
