#include "MidiControllerApp.hpp"

#include "adapters/primary/ui/ViewManager.hpp"
#include "adapters/secondary/hardware/display/Ili9341LvglBridge.hpp"
#include "adapters/secondary/midi/EventEnabledMidiOut.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/debug/DebugMacros.hpp"
#include "config/debug/TaskSchedulerConfig.hpp"
#include "core/TaskScheduler.hpp"
#include "core/domain/events/EventBatcher.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IInputSystem.hpp"
#include "core/domain/interfaces/IMidiSystem.hpp"
#include "core/domain/interfaces/IUISystem.hpp"
#include "core/ports/output/MidiOutputPort.hpp"

MidiControllerApp::MidiControllerApp(std::shared_ptr<DependencyContainer> container)
    : m_container(container) {}

MidiControllerApp::~MidiControllerApp() {
    // Arrêter l'EventBatcher en premier
    if (m_eventBatcher) {
        m_eventBatcher->stop();
        m_eventBatcher.reset();
    }

    // Libération des ressources dans l'ordre inverse
    m_uiSystem.reset();
    m_midiSystem.reset();
    m_inputSystem.reset();
    m_configSystem.reset();
}

Result<bool, std::string> MidiControllerApp::init() {
    // 1. Obtention des références aux sous-systèmes
    m_configSystem = m_container->resolve<IConfiguration>();
    m_inputSystem = m_container->resolve<IInputSystem>();
    m_midiSystem = m_container->resolve<IMidiSystem>();
    m_uiSystem = m_container->resolve<IUISystem>();
    m_lvglBridge = m_container->resolve<Ili9341LvglBridge>();

    if (!m_configSystem || !m_inputSystem || !m_midiSystem || !m_uiSystem) {
        return Result<bool, std::string>::error("Sous-systèmes manquants");
    }
    
    if (!m_lvglBridge) {
        Serial.println(F("AVERTISSEMENT: Bridge LVGL non disponible"));
    }

    // Le décorateur MIDI pour les événements est maintenant créé et géré par MidiSubsystem

    // Initialiser et démarrer l'EventBatcher
    EventBatcher::BatchConfig batchConfig;
    batchConfig.ui_update_interval_ms =
        PerformanceConfig::DISPLAY_REFRESH_PERIOD_MS;  // 60 FPS pour UI
    batchConfig.coalesce_identical_values = true;

    m_eventBatcher = std::make_unique<EventBatcher>(batchConfig);
    m_eventBatcher->start();

    Serial.println(F("EventBatcher initialisé et démarré"));

    return Result<bool, std::string>::success(true);
}

void MidiControllerApp::update() {
    // Mettre à jour le scheduler de tâches en premier
    scheduler.update();

    // Mettre à jour le sous-système d'entrée
    if (m_inputSystem) {
        m_inputSystem->update();
    }

    // Traiter les batchs d'événements UI
    if (m_eventBatcher) {
        m_eventBatcher->processPendingBatches();
    }

    // Mettre à jour le sous-système UI
    if (m_uiSystem) {
        m_uiSystem->update();
    }

    // Rafraîchir l'affichage LVGL avec limitation de fréquence
    static unsigned long last_display_refresh = 0;
    constexpr unsigned long DISPLAY_REFRESH_INTERVAL_MS =
        PerformanceConfig::DISPLAY_REFRESH_PERIOD_MS * 2;  // 60 FPS max

    if (m_lvglBridge && (millis() - last_display_refresh) >= DISPLAY_REFRESH_INTERVAL_MS) {
        m_lvglBridge->refreshDisplay();
        last_display_refresh = millis();
    }
}
