#include <unity.h>
#include <memory>

#include "app/subsystems/MidiSubsystem.hpp"
#include "app/di/DependencyContainer.hpp"
#include "core/domain/interfaces/IConfiguration.hpp"
#include "core/domain/commands/CommandManager.hpp"
#include "adapters/secondary/midi/TeensyUsbMidiOut.hpp"
#include "core/ports/output/MidiOutputPort.hpp"
#include "mocks/MockConfiguration.hpp"
#include "fixtures/test_configurations.hpp"

/**
 * @brief Mock simple pour CommandManager
 * 
 * Mock minimal pour isoler les tests de MidiSubsystem
 * sans dépendre de la logique complexe de gestion des commandes.
 */
class MockCommandManager {
public:
    MockCommandManager() = default;
    ~MockCommandManager() = default;

    // Mock methods pour les tests
    void executeCommand(const std::string& command) {
        execute_calls_++;
        last_command_ = command;
    }

    void scheduleCommand(const std::string& command, unsigned long delay_ms) {
        schedule_calls_++;
        last_scheduled_command_ = command;
        last_delay_ms_ = delay_ms;
    }

    void update() {
        update_calls_++;
    }

    // Méthodes de test
    void reset() {
        execute_calls_ = 0;
        schedule_calls_ = 0;
        update_calls_ = 0;
        last_command_.clear();
        last_scheduled_command_.clear();
        last_delay_ms_ = 0;
    }

    size_t getExecuteCallCount() const { return execute_calls_; }
    size_t getScheduleCallCount() const { return schedule_calls_; }
    size_t getUpdateCallCount() const { return update_calls_; }
    const std::string& getLastCommand() const { return last_command_; }
    const std::string& getLastScheduledCommand() const { return last_scheduled_command_; }
    unsigned long getLastDelayMs() const { return last_delay_ms_; }

private:
    size_t execute_calls_ = 0;
    size_t schedule_calls_ = 0;
    size_t update_calls_ = 0;
    std::string last_command_;
    std::string last_scheduled_command_;
    unsigned long last_delay_ms_ = 0;
};

/**
 * @brief Tests pour MidiSubsystem::init()
 * 
 * Test la logique d'initialisation du sous-système MIDI
 * avec différents scénarios de dépendances et configurations.
 */
class MidiSubsystemTest {
public:
    void setUp() {
        container_ = std::make_shared<DependencyContainer>();
        mockCommandManager_ = std::make_shared<MockCommandManager>();
    }

    void tearDown() {
        container_.reset();
        mockCommandManager_.reset();
    }

    /**
     * @brief Test init() avec dépendances manquantes
     * 
     * Vérifie que l'initialisation échoue gracieusement quand
     * IConfiguration n'est pas disponible dans le conteneur.
     */
    void testInitWithMissingConfiguration() {
        // Arrange - conteneur vide (pas d'IConfiguration)
        auto midiSubsystem = std::make_shared<MidiSubsystem>(container_);
        
        // Act
        auto result = midiSubsystem->init();
        
        // Assert
        TEST_ASSERT_TRUE(result.isError());
        
        auto errorOpt = result.error();
        TEST_ASSERT_TRUE(errorOpt.has_value());
        TEST_ASSERT_EQUAL(ErrorCode::DependencyMissing, errorOpt->code);
    }

    /**
     * @brief Test init() avec configuration valide
     * 
     * Vérifie que l'initialisation réussit avec une configuration
     * minimale et toutes les dépendances de base.
     */
    void testInitWithValidConfiguration() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::MINIMAL);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        auto midiSubsystem = std::make_shared<MidiSubsystem>(container_);
        
        // Act
        auto result = midiSubsystem->init();
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(result.value().value_or(false));
        
        // Vérifier que IMidiSystem a été enregistrée
        auto iMidiSystem = container_->resolve<IMidiSystem>();
        TEST_ASSERT_NOT_NULL(iMidiSystem);
    }

    /**
     * @brief Test init() avec CommandManager préexistant
     * 
     * Vérifie que l'initialisation utilise le CommandManager
     * existant au lieu d'en créer un nouveau.
     */
    void testInitWithExistingCommandManager() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::MINIMAL);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        // Enregistrer un CommandManager existant
        auto mockCommandManagerPtr = std::reinterpret_pointer_cast<CommandManager>(
            std::static_pointer_cast<void>(mockCommandManager_));
        container_->registerDependency<CommandManager>(mockCommandManagerPtr);
        
        auto midiSubsystem = std::make_shared<MidiSubsystem>(container_);
        
        // Act
        auto result = midiSubsystem->init();
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        
        // Vérifier que le même CommandManager est utilisé
        auto commandManager = container_->resolve<CommandManager>();
        TEST_ASSERT_EQUAL_PTR(mockCommandManagerPtr.get(), commandManager.get());
    }

    /**
     * @brief Test double initialisation
     * 
     * Vérifie que l'initialisation multiple retourne succès
     * sans recréer les composants.
     */
    void testDoubleInitialization() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::MINIMAL);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        auto midiSubsystem = std::make_shared<MidiSubsystem>(container_);
        
        // Act - première initialisation
        auto result1 = midiSubsystem->init();
        
        // Act - deuxième initialisation
        auto result2 = midiSubsystem->init();
        
        // Assert
        TEST_ASSERT_TRUE(result1.isSuccess());
        TEST_ASSERT_TRUE(result2.isSuccess());
        TEST_ASSERT_TRUE(result1.value().value_or(false));
        TEST_ASSERT_TRUE(result2.value().value_or(false));
    }

    /**
     * @brief Test sendNoteOn() sans initialisation
     * 
     * Vérifie que sendNoteOn() échoue si le système n'est pas initialisé.
     */
    void testSendNoteOnWithoutInitialization() {
        // Arrange
        auto midiSubsystem = std::make_shared<MidiSubsystem>(container_);
        
        // Act
        auto result = midiSubsystem->sendNoteOn(0, 60, 127);
        
        // Assert
        TEST_ASSERT_TRUE(result.isError());
        
        auto errorOpt = result.error();
        TEST_ASSERT_TRUE(errorOpt.has_value());
        TEST_ASSERT_EQUAL(ErrorCode::OperationFailed, errorOpt->code);
    }

    /**
     * @brief Test sendControlChange() sans initialisation
     * 
     * Vérifie que sendControlChange() échoue si le système n'est pas initialisé.
     */
    void testSendControlChangeWithoutInitialization() {
        // Arrange
        auto midiSubsystem = std::make_shared<MidiSubsystem>(container_);
        
        // Act
        auto result = midiSubsystem->sendControlChange(0, 1, 64);
        
        // Assert
        TEST_ASSERT_TRUE(result.isError());
        
        auto errorOpt = result.error();
        TEST_ASSERT_TRUE(errorOpt.has_value());
        TEST_ASSERT_EQUAL(ErrorCode::OperationFailed, errorOpt->code);
    }

    /**
     * @brief Test update() avec système initialisé
     * 
     * Vérifie que update() ne crash pas et peut être appelé
     * plusieurs fois après initialisation.
     */
    void testUpdateWithInitializedSystem() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::MINIMAL);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        auto midiSubsystem = std::make_shared<MidiSubsystem>(container_);
        midiSubsystem->init();
        
        // Act - update() ne devrait rien faire mais ne pas crasher
        midiSubsystem->update();
        midiSubsystem->update();
        midiSubsystem->update();
        
        // Assert - pas de crash, c'est déjà un succès
        TEST_ASSERT_TRUE(true);
    }

    /**
     * @brief Test configuration complexe
     * 
     * Vérifie que l'initialisation fonctionne avec une configuration
     * plus complexe contenant plusieurs contrôles MIDI.
     */
    void testInitWithComplexConfiguration() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::COMPLEX);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        auto midiSubsystem = std::make_shared<MidiSubsystem>(container_);
        
        // Act
        auto result = midiSubsystem->init();
        
        // Assert
        TEST_ASSERT_TRUE(result.isSuccess());
        TEST_ASSERT_TRUE(result.value().value_or(false));
        
        // Vérifier que les dépendances MIDI ont été créées
        auto midiOutputPort = container_->resolve<MidiOutputPort>();
        TEST_ASSERT_NOT_NULL(midiOutputPort);
        
        auto teensyUsbMidiOut = container_->resolve<TeensyUsbMidiOut>();
        TEST_ASSERT_NOT_NULL(teensyUsbMidiOut);
    }

    /**
     * @brief Test getMidiMapper() après initialisation
     * 
     * Vérifie que getMidiMapper() retourne une référence valide
     * après initialisation du système.
     */
    void testGetMidiMapperAfterInit() {
        // Arrange
        auto mockConfig = std::make_shared<MockConfiguration>(MockConfiguration::ConfigurationType::MINIMAL);
        container_->registerDependency<IConfiguration>(mockConfig);
        
        auto midiSubsystem = std::make_shared<MidiSubsystem>(container_);
        midiSubsystem->init();
        
        // Act & Assert - getMidiMapper() ne devrait pas crasher
        // Note: getMidiMapper() peut retourner une référence valide
        // Nous testons simplement qu'il ne crash pas
        auto& midiMapper = midiSubsystem->getMidiMapper();
        (void)midiMapper;  // Éviter warning unused variable
        TEST_ASSERT_TRUE(true);  // Si on arrive ici, c'est bon
    }

private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<MockCommandManager> mockCommandManager_;
};

// === TESTS GLOBAUX ===

MidiSubsystemTest* midiTestInstance = nullptr;

void setUpMidi(void) {
    midiTestInstance = new MidiSubsystemTest();
    midiTestInstance->setUp();
}

void tearDownMidi(void) {
    if (midiTestInstance) {
        midiTestInstance->tearDown();
        delete midiTestInstance;
        midiTestInstance = nullptr;
    }
}

// === FONCTIONS DE TEST ===

void test_midi_init_with_missing_configuration() {
    setUpMidi();
    midiTestInstance->testInitWithMissingConfiguration();
    tearDownMidi();
}

void test_midi_init_with_valid_configuration() {
    setUpMidi();
    midiTestInstance->testInitWithValidConfiguration();
    tearDownMidi();
}

void test_midi_init_with_existing_command_manager() {
    setUpMidi();
    midiTestInstance->testInitWithExistingCommandManager();
    tearDownMidi();
}

void test_midi_double_initialization() {
    setUpMidi();
    midiTestInstance->testDoubleInitialization();
    tearDownMidi();
}

void test_midi_send_note_on_without_initialization() {
    setUpMidi();
    midiTestInstance->testSendNoteOnWithoutInitialization();
    tearDownMidi();
}

void test_midi_send_control_change_without_initialization() {
    setUpMidi();
    midiTestInstance->testSendControlChangeWithoutInitialization();
    tearDownMidi();
}

void test_midi_update_with_initialized_system() {
    setUpMidi();
    midiTestInstance->testUpdateWithInitializedSystem();
    tearDownMidi();
}

void test_midi_init_with_complex_configuration() {
    setUpMidi();
    midiTestInstance->testInitWithComplexConfiguration();
    tearDownMidi();
}

void test_midi_get_midi_mapper_after_init() {
    setUpMidi();
    midiTestInstance->testGetMidiMapperAfterInit();
    tearDownMidi();
}