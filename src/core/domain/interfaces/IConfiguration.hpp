#pragma once

#include <vector>
#include <optional>

#include "adapters/secondary/hardware/input/InputConfig.hpp"
#include "core/domain/types.hpp"
#include "core/utils/Result.hpp"

/**
 * @brief Interface pour la gestion des configurations
 *
 * Cette interface définit les méthodes que tous les systèmes
 * de configuration doivent implémenter.
 * Interface unifiée basée sur InputConfig.
 */
class IConfiguration {
public:
    virtual ~IConfiguration() = default;

    /**
     * @brief Initialise la configuration
     * @return Résultat de l'initialisation
     */
    virtual Result<bool, std::string> init() = 0;

    // === INTERFACE UNIFIÉE ===
    
    /**
     * @brief Obtient toutes les configurations d'entrée unifiées
     * @return Un vecteur constant de configurations d'entrée
     */
    virtual const std::vector<InputConfig>& getAllInputConfigurations() const = 0;
    
    /**
     * @brief Filtre les configurations par type
     * @param type Type d'entrée à filtrer (BUTTON ou ENCODER)
     * @return Vecteur des configurations du type demandé
     */
    virtual std::vector<InputConfig> getInputConfigurationsByType(InputType type) const = 0;
    
    /**
     * @brief Obtient une configuration par ID
     * @param id Identifiant de l'entrée
     * @return Configuration trouvée ou std::nullopt
     */
    virtual std::optional<InputConfig> getInputConfigurationById(InputId id) const = 0;
    
    /**
     * @brief Filtre les configurations par groupe
     * @param group Nom du groupe à filtrer
     * @return Vecteur des configurations du groupe demandé
     */
    virtual std::vector<InputConfig> getInputConfigurationsByGroup(const std::string& group) const = 0;

    // === MÉTHODES DE NAVIGATION ===
    
    /**
     * @brief Vérifie si un contrôle est dédié à la navigation
     * @param id Identifiant du contrôle à vérifier
     * @return true si le contrôle est utilisé pour la navigation, false sinon
     */
    virtual bool isNavigationControl(InputId id) const = 0;
    
    /**
     * @brief Définit si un contrôle est utilisé pour la navigation
     * @param id Identifiant du contrôle à configurer
     * @param isNavigation true pour définir le contrôle comme contrôle de navigation, false sinon
     */
    virtual void setControlForNavigation(InputId id, bool isNavigation = true) = 0;

    // === MÉTHODES DE CONFIGURATION GÉNÉRALE ===
    
    /**
     * @brief Vérifie si le mode debug est activé
     * @return true si le mode debug est activé, false sinon
     */
    virtual bool isDebugEnabled() const = 0;
    
    /**
     * @brief Obtient le canal MIDI par défaut
     * @return Numéro du canal MIDI (1-16)
     */
    virtual int midiChannel() const = 0;
    
    /**
     * @brief Vérifie si l'initialisation du matériel est activée
     * @return true si l'initialisation du matériel est activée, false sinon
     */
    virtual bool isHardwareInitEnabled() const = 0;
    
    // === MÉTHODES UTILITAIRES ===
    
    /**
     * @brief Valide toutes les configurations
     * @return true si toutes les configurations sont valides
     */
    virtual bool validateAllConfigurations() const = 0;
    
    /**
     * @brief Obtient la liste des groupes disponibles
     * @return Vecteur des noms de groupes uniques
     */
    virtual std::vector<std::string> getAvailableGroups() const = 0;
    
    /**
     * @brief Compte le nombre d'entrées par type
     * @param type Type d'entrée à compter
     * @return Nombre d'entrées du type spécifié
     */
    virtual size_t getInputCountByType(InputType type) const = 0;
};
