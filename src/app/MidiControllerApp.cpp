#include "MidiControllerApp.hpp"

#include "adapters/primary/ui/ViewManager.hpp"
#include "adapters/primary/ui/ViewManagerEventListener.hpp"
#include "adapters/secondary/midi/EventEnabledMidiOut.hpp"
#include "app/di/DependencyContainer.hpp"
#include "app/subsystems/MidiSubsystem.hpp"
#include "config/debug/DebugMacros.hpp"  // Pour avoir accès à PERFORMANCE_MODE
#include "config/debug/TaskSchedulerConfig.hpp"
#include "core/controllers/InputController.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IInputSystem.hpp"
#include "core/domain/interfaces/IMidiSystem.hpp"
#include "core/domain/interfaces/IUISystem.hpp"
#include "core/ports/output/MidiOutputPort.hpp"

MidiControllerApp::MidiControllerApp(std::shared_ptr<DependencyContainer> container)
    : m_container(container) {}

MidiControllerApp::~MidiControllerApp() {
    // Arrêter d'abord l'écouteur d'événements
    if (m_uiEventListener) {
        m_uiEventListener->unsubscribe();
        m_uiEventListener.reset();
    }

    // Nettoyer les ressources dans l'ordre inverse de leur création
    m_eventEnabledMidiOut.reset();
    m_uiSystem.reset();
    m_midiSystem.reset();
    m_inputSystem.reset();
    m_configSystem.reset();
}

Result<bool, std::string> MidiControllerApp::init() {
    // Résoudre les interfaces des sous-systèmes
    m_configSystem = m_container->resolve<IConfiguration>();
    if (!m_configSystem) {
        return Result<bool, std::string>::error("Failed to resolve IConfiguration");
    }

    m_inputSystem = m_container->resolve<IInputSystem>();
    if (!m_inputSystem) {
        return Result<bool, std::string>::error("Failed to resolve IInputSystem");
    }

    m_midiSystem = m_container->resolve<IMidiSystem>();
    if (!m_midiSystem) {
        return Result<bool, std::string>::error("Failed to resolve IMidiSystem");
    }

    m_uiSystem = m_container->resolve<IUISystem>();
    if (!m_uiSystem) {
        return Result<bool, std::string>::error("Failed to resolve IUISystem");
    }

    // Initialiser les sous-systèmes dans l'ordre de dépendance
    auto configResult = m_configSystem->init();
    if (configResult.isError()) {
        return Result<bool, std::string>::error(
            std::string("Failed to initialize configuration subsystem: ") +
            *(configResult.error()));
    }

    auto inputResult = m_inputSystem->init();
    if (inputResult.isError()) {
        return Result<bool, std::string>::error(
            std::string("Failed to initialize input subsystem: ") + *(inputResult.error()));
    }

    auto midiResult = m_midiSystem->init();
    if (midiResult.isError()) {
        return Result<bool, std::string>::error(
            std::string("Failed to initialize MIDI subsystem: ") + *(midiResult.error()));
    }

    auto uiResult = m_uiSystem->init(true);  // true = enable full UI
    if (uiResult.isError()) {
        return Result<bool, std::string>::error(std::string("Failed to initialize UI subsystem: ") +
                                                *(uiResult.error()));
    }

    // ===========================================================================
    // Configuration des adaptateurs pour le système d'événements
    // ===========================================================================

    // 1. Obtenir les ports nécessaires
    auto midiPort = m_container->resolve<MidiOutputPort>();
    auto viewManager = m_container->resolve<ViewManager>();

    if (!midiPort || !viewManager) {
        return Result<bool, std::string>::error(
            "Failed to resolve required ports for event system");
    }

    // 2. Créer le décorateur MidiOutputPort qui émet des événements
    m_eventEnabledMidiOut = std::make_shared<EventEnabledMidiOut>(*midiPort);

    // 3. Remplacer le port MIDI standard par notre version avec événements
    m_container->registerDependency<MidiOutputPort>(m_eventEnabledMidiOut);

    // 4. Créer l'écouteur d'événements UI
    m_uiEventListener = std::make_unique<ViewManagerEventListener>(*viewManager);

    // 5. S'abonner aux événements
    m_uiEventListener->subscribe();

    // 6. Vérifier que l'abonnement a fonctionné
    Serial.print(F("\nUI Event Listener subscription status: "));
    Serial.println(EventBus::getInstance().exists(m_uiEventListener->getSubscriptionId())
                       ? F("SUCCESS")
                       : F("FAILED"));
    Serial.print(F("Total event bus subscribers: "));
    Serial.println(EventBus::getInstance().getCount());

    // ===========================================================================
    // Configuration des callbacks MIDI après l'initialisation de tous les sous-systèmes
    // Note: Cette configuration est centralisée ici pour éviter les redondances
    // avec InitializationScript.hpp et garantir une initialisation cohérente
    // ===========================================================================
    auto inputController = m_container->resolve<InputController>();

    // Utiliser static_cast au lieu de dynamic_pointer_cast car nous savons que m_midiSystem est un
    // MidiSubsystem Ceci est sûr car InitializationScript.hpp crée toujours un MidiSubsystem
    auto midiSubsystem =
        static_cast<MidiSubsystem*>(m_midiSystem.get())
            ? std::shared_ptr<MidiSubsystem>(m_midiSystem,
                                             static_cast<MidiSubsystem*>(m_midiSystem.get()))
            : nullptr;

    if (inputController && midiSubsystem) {
        // Récupérer une référence au MidiMapper
        auto& midiMapper = midiSubsystem->getMidiMapper();

        // 1. Configuration des callbacks directs (chemin critique, contournement du bus
        // d'événements)
        inputController->onEncoderChangedDirect = [&midiMapper](EncoderId id, int32_t position) {
            midiMapper.processEncoderChange(id, position);
        };

        inputController->onEncoderButtonDirect = [&midiMapper](EncoderId id, bool pressed) {
            midiMapper.processEncoderButton(id, pressed);
        };

        inputController->onButtonDirect = [&midiMapper](ButtonId id, bool pressed) {
            midiMapper.processButtonPress(id, pressed);
        };
    } else {
        // Journalisation d'erreur si les composants nécessaires ne sont pas disponibles
        return Result<bool, std::string>::error(
            "Failed to resolve components for MIDI callbacks setup");
    }

    return Result<bool, std::string>::success(true);
}

void MidiControllerApp::update() {
    static unsigned long lastUiUpdateTime = 0;
    unsigned long startTime = micros();

    // Priorité 1: Système d'entrée (critique)
    if (m_inputSystem) {
        m_inputSystem->update();
    }

    unsigned long afterInputTime = micros();
    unsigned long inputDuration = afterInputTime - startTime;

    // Priorité 2: Système MIDI (critique)
    if (m_midiSystem && inputDuration <= TaskTiming::MAX_INPUT_TIME_US) {
        m_midiSystem->update();
    }

    unsigned long afterMidiTime = micros();
    unsigned long midiDuration = afterMidiTime - afterInputTime;

    // Priorité 3: Système UI (moins critique, peut être exécuté à une fréquence plus basse)
    unsigned long currentTime = millis();
    if (m_uiSystem && inputDuration <= TaskTiming::MAX_INPUT_TIME_US &&
        midiDuration <= TaskTiming::MAX_MIDI_TIME_US &&
        currentTime - lastUiUpdateTime >= TaskTiming::UI_MIN_PERIOD_MS) {
        m_uiSystem->update();
        lastUiUpdateTime = currentTime;
    }

// Débordement temporel global à des fins de diagnostic
#ifndef PERFORMANCE_MODE
    unsigned long totalDuration = micros() - startTime;
    static const unsigned long MAX_TOTAL_TIME_US = 5000;  // 5ms
    if (totalDuration > MAX_TOTAL_TIME_US) {
        Serial.printf("WARNING: Update cycle took %lu us (limit: %lu)\n",
                      totalDuration,
                      MAX_TOTAL_TIME_US);
    }
#endif
}