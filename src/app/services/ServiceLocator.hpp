#pragma once

#include "config/ApplicationConfiguration.hpp"
#include "core/domain/IButton.hpp"
#include "core/domain/IEncoder.hpp"
#include "core/domain/IDisplay.hpp"
#include "core/domain/IMidiOut.hpp"
#include "core/domain/IProfileManager.hpp"
#include "core/domain/EventBus.hpp"
#include "core/domain/events/EventTypes.hpp"

#include <memory>
#include <string>
#include <functional>

/**
 * @brief Service d'injection de dépendances
 * 
 * Cette classe centralise la création et l'accès aux objets du système.
 * Elle implémente le pattern Service Locator pour faciliter l'injection de dépendances.
 */
class ServiceLocator {
public:
    /**
     * @brief Initialise le ServiceLocator avec les dépendances principales
     * @param config La configuration de l'application
     */
    static void initialize(const ApplicationConfiguration& config);
    
    /**
     * @brief Récupère la configuration de l'application
     * @return Une référence à la configuration
     */
    static const ApplicationConfiguration& getConfiguration();
    
    /**
     * @brief Récupère le service ConfigurationService
     * @return Une référence au service de configuration
     */
    static class ConfigurationService& getConfigurationService();
    
    /**
     * @brief Récupère le service NavigationConfigService
     * @return Une référence au service de configuration de navigation
     */
    static class NavigationConfigService& getNavigationConfigService();
    
    /**
     * @brief Récupère le service ProfileManager
     * @return Une référence au gestionnaire de profils
     */
    static class ProfileManager& getProfileManager();
    
    /**
     * @brief Récupère le service MidiSystem
     * @return Une référence au système MIDI
     */
    static class MidiSystem& getMidiSystem();
    
    /**
     * @brief Récupère le service InputSystem
     * @return Une référence au système d'entrée
     */
    static class InputSystem& getInputSystem();
    
    /**
     * @brief Récupère le service UiEventService
     * @return Une référence au service d'événements UI
     */
    static class UiEventService& getUiEventService();
    
    /**
     * @brief Enregistre le service ConfigurationService
     * @param service Pointeur vers le service à enregistrer
     */
    static void registerConfigurationService(class ConfigurationService* service);
    
    /**
     * @brief Enregistre le service NavigationConfigService
     * @param service Pointeur vers le service à enregistrer
     */
    static void registerNavigationConfigService(class NavigationConfigService* service);
    
    /**
     * @brief Enregistre le service ProfileManager
     * @param service Pointeur vers le service à enregistrer
     */
    static void registerProfileManager(class ProfileManager* service);
    
    /**
     * @brief Enregistre le service MidiSystem
     * @param service Pointeur vers le service à enregistrer
     */
    static void registerMidiSystem(class MidiSystem* service);
    
    /**
     * @brief Enregistre le service InputSystem
     * @param service Pointeur vers le service à enregistrer
     */
    static void registerInputSystem(class InputSystem* service);
    
    /**
     * @brief Enregistre le service UiEventService
     * @param service Pointeur vers le service à enregistrer
     */
    static void registerUiEventService(class UiEventService* service);
    
    /**
     * @brief Obtient le bus d'événements pour les encodeurs
     * @return Une référence au bus d'événements
     */
    static EventBus<EventTypes::EncoderTurned>& getEncoderTurnedEventBus();
    
    /**
     * @brief Obtient le bus d'événements pour les boutons d'encodeurs
     * @return Une référence au bus d'événements
     */
    static EventBus<EventTypes::EncoderButton>& getEncoderButtonEventBus();
    
    /**
     * @brief Obtient le bus d'événements pour les boutons pressés
     * @return Une référence au bus d'événements
     */
    static EventBus<EventTypes::ButtonPressed>& getButtonPressedEventBus();
    
    /**
     * @brief Obtient le bus d'événements pour les boutons relâchés
     * @return Une référence au bus d'événements
     */
    static EventBus<EventTypes::ButtonReleased>& getButtonReleasedEventBus();
    
private:
    // Singleton pattern
    static ServiceLocator& getInstance();
    
    // Services
    class ConfigurationService* configurationService_;
    class NavigationConfigService* navigationConfigService_;
    class ProfileManager* profileManager_;
    class MidiSystem* midiSystem_;
    class InputSystem* inputSystem_;
    class UiEventService* uiEventService_;
    
    // Configuration
    std::unique_ptr<ApplicationConfiguration> configuration_;
    
    ServiceLocator();
    ~ServiceLocator();
    
    // Empêcher la copie et l'affectation
    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;
};
