#include "MidiControllerApp.hpp"

#include "adapters/secondary/midi/EventEnabledMidiOut.hpp"
#include "adapters/primary/ui/UIEventListener.hpp"
#include "app/di/DependencyContainer.hpp"
#include "app/subsystems/MidiSubsystem.hpp"
#include "core/controllers/InputController.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/interfaces/IInputSystem.hpp"
#include "core/domain/interfaces/IMidiSystem.hpp"
#include "core/domain/interfaces/IUISystem.hpp"
#include "core/domain/events/core/EventBus.hpp"
#include "core/ports/output/MidiOutputPort.hpp"
#include "adapters/primary/ui/ViewManager.hpp"

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
        return Result<bool, std::string>::error("Failed to resolve required ports for event system");
    }
    
    // 2. Créer le décorateur MidiOutputPort qui émet des événements
    m_eventEnabledMidiOut = std::make_shared<EventEnabledMidiOut>(*midiPort);
    
    // 3. Remplacer le port MIDI standard par notre version avec événements
    m_container->registerDependency<MidiOutputPort>(m_eventEnabledMidiOut);
    
    // 4. Créer l'écouteur d'événements UI
    m_uiEventListener = std::make_unique<UIEventListener>(*viewManager);
    
    // 5. S'abonner aux événements
    m_uiEventListener->subscribe();
    
    // 6. Vérifier que l'abonnement a fonctionné
    Serial.print(F("\nUI Event Listener subscription status: "));
    Serial.println(EventBus::getInstance().exists(m_uiEventListener->getSubscriptionId()) ? 
                   F("SUCCESS") : F("FAILED"));
    Serial.print(F("Total event bus subscribers: "));
    Serial.println(EventBus::getInstance().getCount());
    
    // ===========================================================================
    // Configuration des callbacks MIDI après l'initialisation de tous les sous-systèmes
    // Note: Cette configuration est centralisée ici pour éviter les redondances
    // avec InitializationScript.hpp et garantir une initialisation cohérente
    // ===========================================================================
    auto inputController = m_container->resolve<InputController>();
    auto midiSubsystem = std::dynamic_pointer_cast<MidiSubsystem>(m_midiSystem);

    if (inputController && midiSubsystem) {
        // Récupérer une référence au MidiMapper
        auto& midiMapper = midiSubsystem->getMidiMapper();

        // 1. Configuration du callback pour les rotations d'encodeurs
        inputController->setMidiEncoderCallback(
            [&midiMapper](EncoderId id, int32_t position, int8_t delta) {
                midiMapper.processEncoderChange(id, position);
            });

        // 2. Configuration du callback pour les boutons d'encodeurs
        inputController->setMidiEncoderButtonCallback(
            [&midiMapper](EncoderId id, bool pressed) {
                midiMapper.processEncoderButton(id, pressed);
            });

        // 3. Configuration du callback pour les boutons standard
        inputController->setMidiButtonCallback(
            [&midiMapper](ButtonId id, bool pressed) {
                midiMapper.processButtonPress(id, pressed);
            });
    } else {
        // Journalisation d'erreur si les composants nécessaires ne sont pas disponibles
        return Result<bool, std::string>::error("Failed to resolve components for MIDI callbacks setup");
    }

    return Result<bool, std::string>::success(true);
}

void MidiControllerApp::update() {
    // Mettre à jour les sous-systèmes dans l'ordre logique
    if (m_inputSystem) m_inputSystem->update();
    if (m_midiSystem) m_midiSystem->update();
    if (m_uiSystem) m_uiSystem->update();
}