#include <unity.h>
#include <memory>
#include <set>

#include "app/services/NavigationConfigService.hpp"
#include "app/subsystems/InputSubsystem.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "config/unified/ConfigurationFactory.hpp"
#include "core/domain/commands/CommandManager.hpp"
#include "mocks/MockConfiguration.hpp"
#include "mocks/MockMidiOut.hpp"

/**
 * @brief Test de configuration des contrôles de navigation (Phase 1 - REFACTOR)
 * 
 * Vérifie que NavigationConfigService est correctement configuré 
 * par InputSubsystem lors de l'initialisation.
 */

// Variables globales pour les tests
std::shared_ptr<DependencyContainer> testContainer;
std::shared_ptr<NavigationConfigService> navConfigService;
std::shared_ptr<InputSubsystem> inputSubsystem;

void setUpNavigationSync(void) {
    // Initialiser le container de test
    testContainer = std::make_shared<DependencyContainer>();
    
    // Créer NavigationConfigService
    navConfigService = std::make_shared<NavigationConfigService>();
    testContainer->registerDependency<NavigationConfigService>(navConfigService);
    
    // Créer un mock MidiOut
    auto mockMidiOut = std::make_shared<MockMidiOut>();
    testContainer->registerDependency<MidiOutputPort>(mockMidiOut);
    
    // Créer une configuration de test avec les contrôles de navigation définis
    auto configSubsystem = std::make_shared<MockConfiguration>();
    
    // Ajouter manuellement les contrôles de navigation de ConfigurationFactory
    // Menu Button (ID 51)
    ControlDefinition menuButton;
    menuButton.id = 51;
    menuButton.name = "menu_button";
    menuButton.label = "Menu";
    menuButton.group = "Navigation";
    menuButton.enabled = true;
    menuButton.hardware.type = InputType::BUTTON;
    ControlDefinition::MappingSpec menuMapping;
    menuMapping.role = MappingRole::NAVIGATION;
    menuMapping.appliesTo = MappingControlType::BUTTON;
    menuButton.mappings.push_back(menuMapping);
    configSubsystem->addControlDefinition(menuButton);
    
    // OK Button (ID 52)
    ControlDefinition okButton;
    okButton.id = 52;
    okButton.name = "ok_button";
    okButton.label = "OK";
    okButton.group = "Navigation";
    okButton.enabled = true;
    okButton.hardware.type = InputType::BUTTON;
    ControlDefinition::MappingSpec okMapping;
    okMapping.role = MappingRole::NAVIGATION;
    okMapping.appliesTo = MappingControlType::BUTTON;
    okButton.mappings.push_back(okMapping);
    configSubsystem->addControlDefinition(okButton);
    
    // Navigation Encoder (ID 79)
    ControlDefinition navEncoder;
    navEncoder.id = 79;
    navEncoder.name = "nav_encoder";
    navEncoder.label = "Navigation";
    navEncoder.group = "Navigation";
    navEncoder.enabled = true;
    navEncoder.hardware.type = InputType::ENCODER;
    ControlDefinition::MappingSpec navEncoderMapping;
    navEncoderMapping.role = MappingRole::NAVIGATION;
    navEncoderMapping.appliesTo = MappingControlType::ENCODER;
    navEncoder.mappings.push_back(navEncoderMapping);
    configSubsystem->addControlDefinition(navEncoder);
    
    // Navigation Encoder Button (ID 1079)
    ControlDefinition navEncoderButton;
    navEncoderButton.id = 1079;
    navEncoderButton.name = "nav_encoder_button";
    navEncoderButton.label = "Nav Btn";
    navEncoderButton.group = "Navigation";
    navEncoderButton.enabled = true;
    navEncoderButton.hardware.type = InputType::BUTTON;
    navEncoderButton.parentId = 79;  // Child of navigation encoder
    ControlDefinition::MappingSpec navEncoderButtonMapping;
    navEncoderButtonMapping.role = MappingRole::NAVIGATION;
    navEncoderButtonMapping.appliesTo = MappingControlType::BUTTON;
    navEncoderButton.mappings.push_back(navEncoderButtonMapping);
    configSubsystem->addControlDefinition(navEncoderButton);
    
    // Ajouter quelques contrôles MIDI pour les tests de non-navigation
    ControlDefinition midiEncoder;
    midiEncoder.id = 71;
    midiEncoder.name = "midi_encoder_1";
    midiEncoder.label = "Enc 1";
    midiEncoder.group = "MIDI";
    midiEncoder.enabled = true;
    midiEncoder.hardware.type = InputType::ENCODER;
    ControlDefinition::MappingSpec midiMapping;
    midiMapping.role = MappingRole::MIDI;
    midiMapping.appliesTo = MappingControlType::ENCODER;
    midiEncoder.mappings.push_back(midiMapping);
    configSubsystem->addControlDefinition(midiEncoder);
    
    testContainer->registerDependency<IConfiguration>(configSubsystem);
    
    // Créer un CommandManager pour MidiSubsystem
    auto commandManager = std::make_shared<CommandManager>();
    testContainer->registerDependency<CommandManager>(commandManager);
    
    // Créer InputSubsystem
    inputSubsystem = std::make_shared<InputSubsystem>(testContainer);
    testContainer->registerDependency<InputSubsystem>(inputSubsystem);
}

void tearDownNavigationSync(void) {
    // Nettoyer les ressources
    testContainer.reset();
    navConfigService.reset();
    inputSubsystem.reset();
}

/**
 * @brief Test que NavigationConfigService est initialement vide
 */
void test_navigation_config_service_initially_empty(void) {
    setUpNavigationSync();
    // Vérifier que NavigationConfigService ne contient aucun contrôle de navigation
    TEST_ASSERT_FALSE(navConfigService->isNavigationControl(51));  // Menu button
    TEST_ASSERT_FALSE(navConfigService->isNavigationControl(52));  // OK button  
    TEST_ASSERT_FALSE(navConfigService->isNavigationControl(79));  // Navigation encoder
    TEST_ASSERT_FALSE(navConfigService->isNavigationControl(1079)); // Navigation encoder button
    tearDownNavigationSync();
}

/**
 * @brief Test que InputSubsystem configure correctement NavigationConfigService
 */
void test_input_subsystem_configures_navigation_controls(void) {
    setUpNavigationSync();
    // Initialiser NavigationConfigService
    auto navInitResult = navConfigService->init();
    TEST_ASSERT_TRUE(navInitResult);
    
    // Initialiser InputSubsystem
    auto initResult = inputSubsystem->init();
    TEST_ASSERT_TRUE(initResult.isSuccess());
    
    // Vérifier que NavigationConfigService contient les contrôles de navigation
    TEST_ASSERT_TRUE(navConfigService->isNavigationControl(51));   // Menu button
    TEST_ASSERT_TRUE(navConfigService->isNavigationControl(52));   // OK button
    TEST_ASSERT_TRUE(navConfigService->isNavigationControl(79));   // Navigation encoder
    TEST_ASSERT_TRUE(navConfigService->isNavigationControl(1079)); // Navigation encoder button
    
    // Vérifier qu'il y a exactement 4 contrôles de navigation
    TEST_ASSERT_EQUAL(4, navConfigService->getNavigationControlIds().size());
    tearDownNavigationSync();
}

/**
 * @brief Test de l'initialisation automatique des contrôles de navigation
 */
void test_automatic_navigation_controls_initialization(void) {
    setUpNavigationSync();
    // Initialiser NavigationConfigService
    auto navInitResult = navConfigService->init();
    TEST_ASSERT_TRUE(navInitResult);
    
    // Initialiser InputSubsystem qui devrait automatiquement configurer NavigationConfigService
    auto initResult = inputSubsystem->init();
    TEST_ASSERT_TRUE(initResult.isSuccess());
    
    // Vérifier que NavigationConfigService contient automatiquement les contrôles de navigation
    TEST_ASSERT_TRUE(navConfigService->isNavigationControl(51));   // Menu button
    TEST_ASSERT_TRUE(navConfigService->isNavigationControl(52));   // OK button
    TEST_ASSERT_TRUE(navConfigService->isNavigationControl(79));   // Navigation encoder
    TEST_ASSERT_TRUE(navConfigService->isNavigationControl(1079)); // Navigation encoder button
    tearDownNavigationSync();
}

/**
 * @brief Test que les contrôles MIDI normaux ne sont pas marqués comme navigation
 */
void test_midi_controls_not_marked_as_navigation(void) {
    setUpNavigationSync();
    // Initialiser NavigationConfigService
    auto navInitResult = navConfigService->init();
    TEST_ASSERT_TRUE(navInitResult);
    
    // Initialiser InputSubsystem
    auto initResult = inputSubsystem->init();
    TEST_ASSERT_TRUE(initResult.isSuccess());
    
    // Vérifier que les encodeurs MIDI (71-78) ne sont pas marqués comme navigation
    for (int i = 71; i <= 78; i++) {
        TEST_ASSERT_FALSE(navConfigService->isNavigationControl(i));
    }
    
    // Vérifier que les boutons d'encodeurs MIDI (1071-1078) ne sont pas marqués comme navigation
    for (int i = 1071; i <= 1078; i++) {
        TEST_ASSERT_FALSE(navConfigService->isNavigationControl(i));
    }
    tearDownNavigationSync();
}

/**
 * @brief Test de la séparation des responsabilités (navigation vs MIDI)
 */
void test_separation_of_concerns_navigation_vs_midi(void) {
    setUpNavigationSync();
    // Initialiser NavigationConfigService
    auto navInitResult = navConfigService->init();
    TEST_ASSERT_TRUE(navInitResult);
    
    // Initialiser InputSubsystem
    auto initResult = inputSubsystem->init();
    TEST_ASSERT_TRUE(initResult.isSuccess());
    
    // Vérifier que les contrôles de navigation sont bien identifiés
    const auto& navControls = navConfigService->getNavigationControlIds();
    TEST_ASSERT_EQUAL(4, navControls.size());
    
    // Vérifier que chaque contrôle de navigation est correctement identifié
    for (InputId controlId : navControls) {
        TEST_ASSERT_TRUE_MESSAGE(
            navConfigService->isNavigationControl(controlId),
            ("Navigation control " + std::to_string(controlId) + " not properly configured").c_str()
        );
    }
    
    // Vérifier que le contrôle MIDI (71) n'est pas marqué comme navigation
    TEST_ASSERT_FALSE(navConfigService->isNavigationControl(71));
    tearDownNavigationSync();
}

// Point d'entrée pour les tests Unity
void runNavigationControlsSyncTests(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_navigation_config_service_initially_empty);
    RUN_TEST(test_input_subsystem_configures_navigation_controls);
    RUN_TEST(test_automatic_navigation_controls_initialization);
    RUN_TEST(test_midi_controls_not_marked_as_navigation);
    RUN_TEST(test_separation_of_concerns_navigation_vs_midi);
    
    UNITY_END();
}

// Si ce fichier est compilé comme programme principal (pour tests isolés)
#ifdef UNIT_TEST_NAVIGATION_SYNC
int main(void) {
    runNavigationControlsSyncTests();
    return 0;
}
#endif