#include <unity.h>
#include <memory>
#include <set>

#include "app/InitializationScript.hpp"
#include "app/services/NavigationConfigService.hpp"
#include "app/subsystems/MidiSubsystem.hpp"
#include "app/di/DependencyContainer.hpp"
#include "config/ApplicationConfiguration.hpp"
#include "config/unified/ConfigurationFactory.hpp"
#include "mocks/MockConfiguration.hpp"
#include "mocks/MockMidiOut.hpp"

/**
 * @brief Test de synchronisation des contrôles de navigation (Phase 1.1)
 * 
 * Vérifie que NavigationConfigService est correctement synchronisé 
 * avec les contrôles de navigation détectés par MidiSubsystem.
 */

// Variables globales pour les tests
std::shared_ptr<DependencyContainer> testContainer;
std::shared_ptr<NavigationConfigService> navConfigService;
std::shared_ptr<MidiSubsystem> midiSubsystem;

void setUp(void) {
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
    
    // Créer MidiSubsystem
    midiSubsystem = std::make_shared<MidiSubsystem>(testContainer);
    testContainer->registerDependency<MidiSubsystem>(midiSubsystem);
}

void tearDown(void) {
    // Nettoyer les ressources
    testContainer.reset();
    navConfigService.reset();
    midiSubsystem.reset();
}

/**
 * @brief Test que NavigationConfigService est initialement vide
 */
void test_navigation_config_service_initially_empty(void) {
    // Vérifier que NavigationConfigService ne contient aucun contrôle de navigation
    TEST_ASSERT_FALSE(navConfigService->isNavigationControl(51));  // Menu button
    TEST_ASSERT_FALSE(navConfigService->isNavigationControl(52));  // OK button  
    TEST_ASSERT_FALSE(navConfigService->isNavigationControl(79));  // Navigation encoder
    TEST_ASSERT_FALSE(navConfigService->isNavigationControl(1079)); // Navigation encoder button
}

/**
 * @brief Test que MidiSubsystem détecte correctement les contrôles de navigation
 */
void test_midi_subsystem_detects_navigation_controls(void) {
    // Initialiser MidiSubsystem
    auto initResult = midiSubsystem->init();
    TEST_ASSERT_TRUE(initResult.isSuccess());
    
    // Obtenir MidiMapper
    auto& midiMapper = midiSubsystem->getMidiMapper();
    
    // Vérifier que MidiMapper a détecté les contrôles de navigation
    const auto& navigationControlIds = midiMapper.getNavigationControlIds();
    
    // Vérifier que les contrôles de navigation définis dans ConfigurationFactory sont détectés
    TEST_ASSERT_TRUE(navigationControlIds.find(51) != navigationControlIds.end());   // Menu button
    TEST_ASSERT_TRUE(navigationControlIds.find(52) != navigationControlIds.end());   // OK button
    TEST_ASSERT_TRUE(navigationControlIds.find(79) != navigationControlIds.end());   // Navigation encoder
    TEST_ASSERT_TRUE(navigationControlIds.find(1079) != navigationControlIds.end()); // Navigation encoder button
    
    // Vérifier qu'il y a exactement 4 contrôles de navigation
    TEST_ASSERT_EQUAL(4, navigationControlIds.size());
}

/**
 * @brief Test de la synchronisation entre MidiSubsystem et NavigationConfigService
 */
void test_synchronization_navigation_controls(void) {
    // Initialiser MidiSubsystem
    auto initResult = midiSubsystem->init();
    TEST_ASSERT_TRUE(initResult.isSuccess());
    
    // Effectuer la synchronisation
    InitializationScript::syncNavigationControlsWithConfigService(midiSubsystem, navConfigService);
    
    // Vérifier que NavigationConfigService contient maintenant les contrôles de navigation
    TEST_ASSERT_TRUE(navConfigService->isNavigationControl(51));   // Menu button
    TEST_ASSERT_TRUE(navConfigService->isNavigationControl(52));   // OK button
    TEST_ASSERT_TRUE(navConfigService->isNavigationControl(79));   // Navigation encoder
    TEST_ASSERT_TRUE(navConfigService->isNavigationControl(1079)); // Navigation encoder button
}

/**
 * @brief Test que les contrôles MIDI normaux ne sont pas marqués comme navigation
 */
void test_midi_controls_not_marked_as_navigation(void) {
    // Initialiser MidiSubsystem
    auto initResult = midiSubsystem->init();
    TEST_ASSERT_TRUE(initResult.isSuccess());
    
    // Effectuer la synchronisation
    InitializationScript::syncNavigationControlsWithConfigService(midiSubsystem, navConfigService);
    
    // Vérifier que les encodeurs MIDI (71-78) ne sont pas marqués comme navigation
    for (int i = 71; i <= 78; i++) {
        TEST_ASSERT_FALSE(navConfigService->isNavigationControl(i));
    }
    
    // Vérifier que les boutons d'encodeurs MIDI (1071-1078) ne sont pas marqués comme navigation
    for (int i = 1071; i <= 1078; i++) {
        TEST_ASSERT_FALSE(navConfigService->isNavigationControl(i));
    }
}

/**
 * @brief Test de cohérence entre MidiMapper et NavigationConfigService après synchronisation
 */
void test_consistency_between_midi_mapper_and_navigation_service(void) {
    // Initialiser MidiSubsystem
    auto initResult = midiSubsystem->init();
    TEST_ASSERT_TRUE(initResult.isSuccess());
    
    // Effectuer la synchronisation
    InitializationScript::syncNavigationControlsWithConfigService(midiSubsystem, navConfigService);
    
    // Obtenir les contrôles de navigation du MidiMapper
    auto& midiMapper = midiSubsystem->getMidiMapper();
    const auto& midiMapperNavControls = midiMapper.getNavigationControlIds();
    
    // Vérifier que NavigationConfigService et MidiMapper sont cohérents
    for (InputId controlId : midiMapperNavControls) {
        TEST_ASSERT_TRUE_MESSAGE(
            navConfigService->isNavigationControl(controlId),
            ("Navigation control " + std::to_string(controlId) + " not found in NavigationConfigService").c_str()
        );
    }
    
    // Vérifier qu'il n'y a pas de contrôles supplémentaires dans NavigationConfigService
    // (cette vérification serait plus complexe à implémenter car NavigationConfigService 
    // n'expose pas directement ses contrôles - c'est un design volontaire)
}

// Point d'entrée pour les tests Unity
void runNavigationControlsSyncTests(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_navigation_config_service_initially_empty);
    RUN_TEST(test_midi_subsystem_detects_navigation_controls);
    RUN_TEST(test_synchronization_navigation_controls);
    RUN_TEST(test_midi_controls_not_marked_as_navigation);
    RUN_TEST(test_consistency_between_midi_mapper_and_navigation_service);
    
    UNITY_END();
}

// Si ce fichier est compilé comme programme principal (pour tests isolés)
#ifdef UNIT_TEST_NAVIGATION_SYNC
int main(void) {
    runNavigationControlsSyncTests();
    return 0;
}
#endif