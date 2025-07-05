#pragma once

#include "core/domain/types.hpp"
#include <set>
#include <vector>

/**
 * @brief Interface pour la gestion des contrôles de navigation
 * 
 * REFACTOR: Interface créée pour découpler la logique de navigation
 * du système MIDI et permettre une architecture plus propre.
 * 
 * Cette interface centralise la gestion des contrôles dédiés à la navigation
 * dans l'interface utilisateur (boutons menu, encodeur navigation, etc.)
 * et les sépare clairement des contrôles MIDI.
 */
class INavigationService {
public:
    virtual ~INavigationService() = default;

    /**
     * @brief Définit un contrôle comme étant dédié à la navigation
     * @param id Identifiant du contrôle
     * @param isNavigation Indique si le contrôle est pour la navigation (true) ou pas (false)
     */
    virtual void setControlForNavigation(InputId id, bool isNavigation = true) = 0;

    /**
     * @brief Vérifie si un contrôle est dédié à la navigation
     * @param id Identifiant du contrôle à vérifier
     * @return true si le contrôle est dédié à la navigation, false sinon
     */
    virtual bool isNavigationControl(InputId id) const = 0;

    /**
     * @brief Configure plusieurs contrôles de navigation en une fois
     * @param navigationControlIds Ensemble des IDs des contrôles de navigation
     */
    virtual void setNavigationControls(const std::set<InputId>& navigationControlIds) = 0;

    /**
     * @brief Obtient tous les contrôles de navigation configurés
     * @return Ensemble des IDs des contrôles de navigation
     */
    virtual const std::set<InputId>& getNavigationControlIds() const = 0;

    /**
     * @brief Réinitialise la configuration des contrôles de navigation
     */
    virtual void clearNavigationControls() = 0;

    /**
     * @brief Vérifie si le service de navigation est initialisé
     * @return true si initialisé, false sinon
     */
    virtual bool isInitialized() const = 0;
};