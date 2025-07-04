#include <unity.h>
#include "adapters/primary/ui/parameter/LvglSceneManager.hpp"
#include "adapters/primary/ui/parameter/WidgetMappingManager.hpp"
#include "adapters/primary/ui/parameter/MidiConfigurationParser.hpp"

// Mock pour LVGL (car pas disponible dans l'environnement de test)
extern "C" {
    // Structures LVGL minimales pour les tests
    struct _lv_obj_t {
        int dummy;
    };
    
    _lv_obj_t* lv_obj_create(_lv_obj_t* parent) {
        return reinterpret_cast<_lv_obj_t*>(0x1000 + rand() % 1000);
    }
    
    void lv_obj_delete(_lv_obj_t* obj) {
        // Mock delete
    }
    
    void lv_obj_set_size(_lv_obj_t* obj, int w, int h) {}
    void lv_obj_set_pos(_lv_obj_t* obj, int x, int y) {}
    void lv_obj_set_style_bg_color(_lv_obj_t* obj, int color, int state) {}
    void lv_obj_set_style_bg_opa(_lv_obj_t* obj, int opa, int state) {}
    void lv_obj_set_style_pad_all(_lv_obj_t* obj, int pad, int state) {}
    void lv_obj_set_style_border_width(_lv_obj_t* obj, int width, int state) {}
    void lv_obj_set_style_pad_gap(_lv_obj_t* obj, int gap, int state) {}
    void lv_obj_set_grid_dsc_array(_lv_obj_t* obj, void* col_dsc, void* row_dsc) {}
    void lv_obj_set_layout(_lv_obj_t* obj, int layout) {}
    void lv_obj_set_grid_cell(_lv_obj_t* obj, int align_col, int col, int span_col, int align_row, int row, int span_row) {}
    void lv_obj_center(_lv_obj_t* obj) {}
    void lv_obj_move_foreground(_lv_obj_t* obj) {}
    void lv_obj_invalidate(_lv_obj_t* obj) {}
    
    int lv_color_hex(int color) { return color; }
}

class TestLvglSceneManager {
public:
    static std::shared_ptr<WidgetMappingManager> mockMappingManager;
    
    static void setUp() {
        // Créer un mapping manager configuré pour les tests
        mockMappingManager = std::make_shared<WidgetMappingManager>();
        
        // Configuration de test avec CC1-3 mappés aux widgets 0-2
        std::vector<MidiConfigurationParser::MidiControlInfo> midiControls;
        for (int i = 0; i < 3; i++) {
            MidiConfigurationParser::MidiControlInfo info;
            info.cc_number = i + 1;  // CC1, CC2, CC3
            info.channel = 0;
            info.name = "ENC" + String(i + 1);
            info.control_id = 71 + i;
            midiControls.push_back(info);
        }
        
        // Bouton test mappé au widget 0
        std::vector<MidiConfigurationParser::ButtonInfo> buttonInfos;
        MidiConfigurationParser::ButtonInfo buttonInfo;
        buttonInfo.button_id = 1071;
        buttonInfo.parent_encoder_id = 71;  // Parent de CC1
        buttonInfo.name = "ENC1 BTN";
        buttonInfos.push_back(buttonInfo);
        
        mockMappingManager->initializeMappings(midiControls, buttonInfos);
    }
    
    static void tearDown() {
        mockMappingManager.reset();
    }

    static void test_constructor_default_config() {
        LvglSceneManager::SceneConfig config;
        LvglSceneManager manager(config);
        
        TEST_ASSERT_FALSE(manager.isInitialized());
        TEST_ASSERT_NULL(manager.getMainScreen());
        TEST_ASSERT_NULL(manager.getGridContainer());
        TEST_ASSERT_EQUAL(0, manager.getWidgetCount());
        
        auto stats = manager.getStats();
        TEST_ASSERT_FALSE(stats.sceneInitialized);
        TEST_ASSERT_EQUAL(0, stats.widgetsCreated);
    }

    static void test_constructor_custom_config() {
        LvglSceneManager::SceneConfig config;
        config.maxWidgets = 4;
        config.screenWidth = 480;
        config.screenHeight = 320;
        config.enableLogging = true;
        
        LvglSceneManager manager(config, mockMappingManager);
        
        TEST_ASSERT_FALSE(manager.isInitialized());
        TEST_ASSERT_EQUAL(0, manager.getWidgetCount());
    }

    static void test_initialize_scene_success() {
        LvglSceneManager::SceneConfig config;
        config.enableLogging = false;  // Éviter le spam dans les tests
        LvglSceneManager manager(config);
        
        bool result = manager.initializeScene();
        
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_TRUE(manager.isInitialized());
        TEST_ASSERT_NOT_NULL(manager.getMainScreen());
        TEST_ASSERT_NOT_NULL(manager.getGridContainer());
        TEST_ASSERT_EQUAL(8, manager.getWidgetCount());
        
        auto stats = manager.getStats();
        TEST_ASSERT_TRUE(stats.sceneInitialized);
        TEST_ASSERT_EQUAL(8, stats.widgetsCreated);
        TEST_ASSERT_GREATER_THAN(0, stats.memoryUsageEstimate);
    }

    static void test_initialize_scene_with_widget_config() {
        LvglSceneManager::SceneConfig config;
        config.enableLogging = false;
        LvglSceneManager manager(config);
        
        // Créer un accesseur de configuration pour les widgets
        auto widgetConfigAccessor = [](uint8_t index) -> LvglSceneManager::WidgetConfig* {
            static LvglSceneManager::WidgetConfig configs[8];
            if (index < 3) {
                configs[index].cc_number = index + 10;  // CC10, CC11, CC12
                configs[index].channel = 1;
                configs[index].value = 64;
                configs[index].name = "Test" + String(index);
                configs[index].visible = true;
                return &configs[index];
            }
            return nullptr;  // Pas de config pour les autres widgets
        };
        
        bool result = manager.initializeScene(widgetConfigAccessor);
        
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_TRUE(manager.isInitialized());
        
        // Vérifier que les widgets sont créés
        for (uint8_t i = 0; i < 8; i++) {
            ParameterWidget* widget = manager.getWidget(i);
            TEST_ASSERT_NOT_NULL(widget);
        }
    }

    static void test_initialize_scene_twice() {
        LvglSceneManager::SceneConfig config;
        config.enableLogging = false;
        LvglSceneManager manager(config);
        
        bool result1 = manager.initializeScene();
        bool result2 = manager.initializeScene();  // Deuxième appel
        
        TEST_ASSERT_TRUE(result1);
        TEST_ASSERT_TRUE(result2);  // Doit retourner true sans re-initialiser
        TEST_ASSERT_TRUE(manager.isInitialized());
    }

    static void test_cleanup() {
        LvglSceneManager::SceneConfig config;
        config.enableLogging = false;
        LvglSceneManager manager(config);
        
        manager.initializeScene();
        TEST_ASSERT_TRUE(manager.isInitialized());
        
        manager.cleanup();
        
        TEST_ASSERT_FALSE(manager.isInitialized());
        TEST_ASSERT_NULL(manager.getMainScreen());
        TEST_ASSERT_NULL(manager.getGridContainer());
        
        auto stats = manager.getStats();
        TEST_ASSERT_FALSE(stats.sceneInitialized);
    }

    static void test_widgets_visibility() {
        LvglSceneManager::SceneConfig config;
        config.enableLogging = false;
        LvglSceneManager manager(config);
        
        manager.initializeScene();
        
        // Tester la visibilité des widgets
        manager.setWidgetsVisible(false);
        manager.setWidgetsVisible(true);
        
        // Pas de crash = succès pour ce test
        TEST_ASSERT_TRUE(true);
    }

    static void test_get_widget_valid_index() {
        LvglSceneManager::SceneConfig config;
        config.enableLogging = false;
        LvglSceneManager manager(config);
        
        manager.initializeScene();
        
        for (uint8_t i = 0; i < 8; i++) {
            ParameterWidget* widget = manager.getWidget(i);
            TEST_ASSERT_NOT_NULL(widget);
        }
    }

    static void test_get_widget_invalid_index() {
        LvglSceneManager::SceneConfig config;
        config.enableLogging = false;
        LvglSceneManager manager(config);
        
        manager.initializeScene();
        
        ParameterWidget* widget = manager.getWidget(10);  // Index invalide
        TEST_ASSERT_NULL(widget);
    }

    static void test_get_widget_before_init() {
        LvglSceneManager::SceneConfig config;
        LvglSceneManager manager(config);
        
        ParameterWidget* widget = manager.getWidget(0);
        TEST_ASSERT_NULL(widget);
    }

    static void test_finalize_positioning() {
        LvglSceneManager::SceneConfig config;
        config.enableLogging = false;
        LvglSceneManager manager(config, mockMappingManager);
        
        manager.initializeScene();
        manager.finalizePositioning();
        
        // Pas de crash = succès
        TEST_ASSERT_TRUE(manager.isInitialized());
    }

    static void test_update_mapping_manager() {
        LvglSceneManager::SceneConfig config;
        config.enableLogging = false;
        LvglSceneManager manager(config);
        
        manager.initializeScene();
        
        // Mettre à jour le mapping manager
        auto newMappingManager = std::make_shared<WidgetMappingManager>();
        manager.updateMappingManager(newMappingManager);
        
        TEST_ASSERT_TRUE(manager.isInitialized());
    }

    static void test_stats_functionality() {
        LvglSceneManager::SceneConfig config;
        config.enableLogging = false;
        LvglSceneManager manager(config, mockMappingManager);
        
        // Stats avant initialisation
        auto statsBefore = manager.getStats();
        TEST_ASSERT_FALSE(statsBefore.sceneInitialized);
        TEST_ASSERT_EQUAL(0, statsBefore.widgetsCreated);
        
        // Initialiser et vérifier les stats
        manager.initializeScene();
        auto statsAfter = manager.getStats();
        
        TEST_ASSERT_TRUE(statsAfter.sceneInitialized);
        TEST_ASSERT_EQUAL(8, statsAfter.widgetsCreated);
        TEST_ASSERT_EQUAL(1, statsAfter.buttonIndicatorsCreated);  // Un bouton mappé
        TEST_ASSERT_GREATER_THAN(0, statsAfter.memoryUsageEstimate);
    }

    static void test_custom_widget_count() {
        LvglSceneManager::SceneConfig config;
        config.maxWidgets = 4;  // Seulement 4 widgets
        config.enableLogging = false;
        LvglSceneManager manager(config);
        
        manager.initializeScene();
        
        TEST_ASSERT_EQUAL(4, manager.getWidgetCount());
        
        // Vérifier que les 4 premiers widgets existent
        for (uint8_t i = 0; i < 4; i++) {
            TEST_ASSERT_NOT_NULL(manager.getWidget(i));
        }
        
        // Vérifier que les widgets 4-7 n'existent pas ou sont nuls
        for (uint8_t i = 4; i < 8; i++) {
            TEST_ASSERT_NULL(manager.getWidget(i));
        }
    }

    static void test_destructor_cleanup() {
        LvglSceneManager::SceneConfig config;
        config.enableLogging = false;
        
        {
            LvglSceneManager manager(config);
            manager.initializeScene();
            TEST_ASSERT_TRUE(manager.isInitialized());
            // Le destructeur sera appelé à la fin de ce scope
        }
        
        // Si nous arrivons ici sans crash, le destructeur a fonctionné
        TEST_ASSERT_TRUE(true);
    }
};

// Définitions statiques
std::shared_ptr<WidgetMappingManager> TestLvglSceneManager::mockMappingManager;

void test_lvgl_scene_manager_constructor_default() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_constructor_default_config();
    TestLvglSceneManager::tearDown();
}

void test_lvgl_scene_manager_constructor_custom() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_constructor_custom_config();
    TestLvglSceneManager::tearDown();
}

void test_lvgl_scene_manager_initialize_success() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_initialize_scene_success();
    TestLvglSceneManager::tearDown();
}

void test_lvgl_scene_manager_initialize_with_config() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_initialize_scene_with_widget_config();
    TestLvglSceneManager::tearDown();
}

void test_lvgl_scene_manager_initialize_twice() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_initialize_scene_twice();
    TestLvglSceneManager::tearDown();
}

void test_lvgl_scene_manager_cleanup() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_cleanup();
    TestLvglSceneManager::tearDown();
}

void test_lvgl_scene_manager_widgets_visibility() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_widgets_visibility();
    TestLvglSceneManager::tearDown();
}

void test_lvgl_scene_manager_get_widget_valid() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_get_widget_valid_index();
    TestLvglSceneManager::tearDown();
}

void test_lvgl_scene_manager_get_widget_invalid() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_get_widget_invalid_index();
    TestLvglSceneManager::tearDown();
}

void test_lvgl_scene_manager_get_widget_before_init() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_get_widget_before_init();
    TestLvglSceneManager::tearDown();
}

void test_lvgl_scene_manager_finalize_positioning() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_finalize_positioning();
    TestLvglSceneManager::tearDown();
}

void test_lvgl_scene_manager_update_mapping() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_update_mapping_manager();
    TestLvglSceneManager::tearDown();
}

void test_lvgl_scene_manager_stats() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_stats_functionality();
    TestLvglSceneManager::tearDown();
}

void test_lvgl_scene_manager_custom_widget_count() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_custom_widget_count();
    TestLvglSceneManager::tearDown();
}

void test_lvgl_scene_manager_destructor() {
    TestLvglSceneManager::setUp();
    TestLvglSceneManager::test_destructor_cleanup();
    TestLvglSceneManager::tearDown();
}