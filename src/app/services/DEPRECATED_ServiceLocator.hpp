#pragma once

// ===================================================================
// CE FICHIER EST CONSERVÉ UNIQUEMENT POUR RÉFÉRENCE HISTORIQUE
// Il ne doit pas être inclus dans le projet
// Utilisez DependencyContainer et ServiceLocatorAdapter à la place
// ===================================================================

#include <memory>

// Forward declarations
class ApplicationConfiguration;
class ConfigurationService;
class NavigationConfigService;
class ProfileManager;
class MidiSystem;
class InputSystem;
class UiEventService;
class ControllerService;
class InputController;
class UIController;
class UIEventListener;
class ViewManager;
class MenuController;
class EventBus;

/**
 * @brief Ce fichier représente l'ancien ServiceLocator qui a été remplacé
 * par DependencyContainer et ServiceLocatorAdapter.
 * 
 * Il est conservé uniquement pour référence historique et documentation.
 * 
 * @deprecated Remplacé par DependencyContainer et ServiceLocatorAdapter.
 */
class [[deprecated]] ServiceLocator {
    // Classe supprimée dans la Phase 8 du refactoring
    // Voir docs/MigrationGuide.md pour plus d'informations
};
