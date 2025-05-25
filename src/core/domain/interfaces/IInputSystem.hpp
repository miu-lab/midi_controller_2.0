#pragma once

#include <vector>
#include <optional>

#include "config/unified/ControlDefinition.hpp"
#include "core/utils/Result.hpp"

/**
 * @brief Interface pour le système d'entrée
 *
 * Cette interface définit les méthodes que tous les systèmes d'entrée doivent implémenter.
 * Elle permet de gérer les périphériques d'entrée de manière unifiée avec ControlDefinition.
 */
class IInputSystem {
public:
    virtual ~IInputSystem() = default;

    /**
     * @brief Initialise le système d'entrée
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    virtual Result<bool, std::string> init() = 0;

    /**
     * @brief Met à jour l'état du système d'entrée
     * 
     * Cette méthode doit être appelée régulièrement pour traiter les événements d'entrée.
     */
    virtual void update() = 0;

    // === INTERFACE UNIFIÉE ===
    
    /**
     * @brief Configure toutes les entrées avec les définitions de contrôles fournies
     * @param controlDefinitions Definitions de contrôles unifiées
     * @return Result<bool, std::string> Succès ou message d'erreur
     */
    virtual Result<bool, std::string> configureInputs(const std::vector<ControlDefinition>& controlDefinitions) = 0;
    
    /**
     * @brief Obtient toutes les définitions de contrôles actives
     * @return Vecteur des définitions de contrôles
     */
    virtual std::vector<ControlDefinition> getAllActiveControlDefinitions() const = 0;
    
    /**
     * @brief Obtient une définition de contrôle par ID
     * @param id Identifiant de l'entrée
     * @return Définition trouvée ou std::nullopt
     */
    virtual std::optional<ControlDefinition> getControlDefinitionById(InputId id) const = 0;
    
    /**
     * @brief Obtient le nombre d'entrées actives par type
     * @param type Type d'entrée à compter
     * @return Nombre d'entrées actives du type spécifié
     */
    virtual size_t getActiveInputCountByType(InputType type) const = 0;
    
    /**
     * @brief Vérifie si toutes les entrées sont initialisées correctement
     * @return true si toutes les entrées sont fonctionnelles
     */
    virtual bool validateInputsStatus() const = 0;
};
