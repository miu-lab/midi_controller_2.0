#pragma once

#include <vector>
#include <optional>

#include "config/unified/ControlDefinition.hpp"
#include "core/domain/types.hpp"
#include "core/utils/Result.hpp"

/**
 * @brief Interface pour la gestion des configurations
 *
 * Cette interface définit les méthodes que tous les systèmes
 * de configuration doivent implémenter.
 * Interface unifiée basée sur ControlDefinition.
 */
class IConfiguration {
public:
    virtual ~IConfiguration() = default;

    /**
     * @brief Initialise la configuration
     * @return Résultat de l'initialisation
     */
    virtual Result<bool> init() = 0;

    // === INTERFACE UNIFIÉE ===
    
    /**
     * @brief Obtient toutes les définitions de contrôles unifiées
     * @return Un vecteur constant de définitions de contrôles
     */
    virtual const std::vector<ControlDefinition>& getAllControlDefinitions() const = 0;
    
    /**
     * @brief Filtre les définitions par type
     * @param type Type d'entrée à filtrer (BUTTON ou ENCODER)
     * @return Vecteur des définitions du type demandé
     */
    virtual std::vector<ControlDefinition> getControlDefinitionsByType(InputType type) const = 0;
    
    /**
     * @brief Obtient une définition par ID
     * @param id Identifiant de l'entrée
     * @return Définition trouvée ou std::nullopt
     */
    virtual std::optional<ControlDefinition> getControlDefinitionById(InputId id) const = 0;
    
    /**
     * @brief Filtre les définitions par groupe
     * @param group Nom du groupe à filtrer
     * @return Vecteur des définitions du groupe demandé
     */
    virtual std::vector<ControlDefinition> getControlDefinitionsByGroup(const std::string& group) const = 0;

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
     * @brief Valide toutes les définitions de contrôles
     * @return true si toutes les définitions sont valides
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