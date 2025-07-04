#include <unity.h>
#include "adapters/primary/ui/parameter/ParameterViewController.hpp"
#include "adapters/primary/ui/parameter/MidiConfigurationParser.hpp"
#include "core/domain/events/MidiEvents.hpp"
#include "core/domain/events/UIEvent.hpp"
#include "core/domain/events/core/EventTypes.hpp"

// Mock pour LVGL (car pas disponible dans l'environnement de test)
extern "C" {
    struct _lv_obj_t { int dummy; };
    
    _lv_obj_t* lv_obj_create(_lv_obj_t* parent) {
        return reinterpret_cast<_lv_obj_t*>(0x1000 + rand() % 1000);
    }
    
    void lv_obj_delete(_lv_obj_t* obj) {}
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
    void lv_screen_load(_lv_obj_t* screen) {}
    
    int lv_color_hex(int color) { return color; }
}

// Mock classes pour les dépendances (héritage pour polymorphisme)
class MockIli9341LvglBridge : public Ili9341LvglBridge {
public:
    MockIli9341LvglBridge() : Ili9341LvglBridge(nullptr, 0, 0, 0, 0, 0, 0, 0) {}
    void refresh() override {}
    void init() override {}
    void clear() override {}
};

class MockUnifiedConfiguration : public UnifiedConfiguration {
public:
    MockUnifiedConfiguration() = default;
    // Les méthodes virtuelles seront héritées
};

class MockEventBus : public EventBus {
public:
    MockEventBus() : next_id_(1) {}
    
    SubscriptionId subscribeHigh(EventListener* listener) override {
        listeners_[next_id_] = listener;
        return next_id_++;
    }
    
    void unsubscribe(SubscriptionId id) override {
        listeners_.erase(id);
    }
    
    void publishEvent(const Event& event) {
        for (auto& pair : listeners_) {
            if (pair.second) {
                pair.second->onEvent(event);
            }
        }
    }
    
private:
    SubscriptionId next_id_;
    std::map<SubscriptionId, EventListener*> listeners_;
};

class TestParameterViewController {
public:
    static std::shared_ptr<MockIli9341LvglBridge> mockBridge;
    static std::shared_ptr<MockUnifiedConfiguration> mockConfig;
    static std::shared_ptr<MockEventBus> mockEventBus;
    
    static void setUp() {
        mockBridge = std::make_shared<MockIli9341LvglBridge>();
        mockConfig = std::make_shared<MockUnifiedConfiguration>();
        mockEventBus = std::make_shared<MockEventBus>();
    }
    
    static void tearDown() {
        mockBridge.reset();
        mockConfig.reset();
        mockEventBus.reset();
    }
    
    static void test_constructor_default_config() {
        ParameterViewController::ControllerConfig config;
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        
        TEST_ASSERT_FALSE(controller.isInitialized());
        TEST_ASSERT_FALSE(controller.isActive());
        TEST_ASSERT_NULL(controller.getMainScreen());
    }
    
    static void test_constructor_custom_config() {
        ParameterViewController::ControllerConfig config;
        config.maxWidgets = 4;
        config.screenWidth = 480;
        config.screenHeight = 320;
        config.enableLogging = false;
        config.enableAnimation = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        
        TEST_ASSERT_FALSE(controller.isInitialized());
        TEST_ASSERT_FALSE(controller.isActive());
    }
    
    static void test_initialize_success() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;  // Éviter le spam dans les tests
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        
        bool result = controller.initialize();
        
        TEST_ASSERT_TRUE(result);
        TEST_ASSERT_TRUE(controller.isInitialized());
        TEST_ASSERT_NOT_NULL(controller.getMainScreen());
    }
    
    static void test_initialize_twice() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        
        bool result1 = controller.initialize();
        bool result2 = controller.initialize();  // Deuxième appel
        
        TEST_ASSERT_TRUE(result1);
        TEST_ASSERT_TRUE(result2);  // Doit retourner true sans re-initialiser
        TEST_ASSERT_TRUE(controller.isInitialized());
    }
    
    static void test_initialize_missing_dependencies() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        // Test avec bridge manquant
        ParameterViewController controller1(config, nullptr, mockConfig, mockEventBus);
        TEST_ASSERT_FALSE(controller1.initialize());
        
        // Test avec config manquante
        ParameterViewController controller2(config, mockBridge, nullptr, mockEventBus);
        TEST_ASSERT_FALSE(controller2.initialize());
        
        // Test avec eventBus manquant
        ParameterViewController controller3(config, mockBridge, mockConfig, nullptr);
        TEST_ASSERT_FALSE(controller3.initialize());
    }
    
    static void test_activation() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        controller.initialize();
        
        TEST_ASSERT_FALSE(controller.isActive());
        
        controller.setActive(true);
        TEST_ASSERT_TRUE(controller.isActive());
        
        controller.setActive(false);
        TEST_ASSERT_FALSE(controller.isActive());
        
        // Test activation multiple sans changement
        controller.setActive(true);
        controller.setActive(true);  // Pas de changement
        TEST_ASSERT_TRUE(controller.isActive());
    }
    
    static void test_update_before_init() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        
        // Update avant initialisation ne doit pas crasher
        controller.update();
        TEST_ASSERT_TRUE(true);  // Si on arrive ici, pas de crash
    }
    
    static void test_update_while_inactive() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        controller.initialize();
        
        // Update quand inactif ne doit pas crasher
        controller.update();
        TEST_ASSERT_TRUE(true);
    }
    
    static void test_update_while_active() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        controller.initialize();
        controller.setActive(true);
        
        // Update quand actif
        controller.update();
        TEST_ASSERT_TRUE(controller.isActive());
    }
    
    static void test_set_parameter() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        controller.initialize();
        controller.setActive(true);
        
        // Test avec CC qui pourrait exister
        controller.setParameter(1, 1, 64, "Test Param", true);
        
        // Pas de crash = succès
        TEST_ASSERT_TRUE(controller.isActive());
    }
    
    static void test_set_button_state() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        controller.initialize();
        controller.setActive(true);
        
        // Test avec bouton qui pourrait exister
        controller.setButtonState(1071, true, true);
        controller.setButtonState(1071, false, false);
        
        // Pas de crash = succès
        TEST_ASSERT_TRUE(controller.isActive());
    }
    
    static void test_get_widget_for_cc() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        controller.initialize();
        
        // Test avec CC inexistant
        ParameterWidget* widget = controller.getWidgetForCC(99);
        TEST_ASSERT_NULL(widget);  // Aucun mapping pour CC99
    }
    
    static void test_get_widget_for_button() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        controller.initialize();
        
        // Test avec bouton inexistant
        ParameterWidget* widget = controller.getWidgetForButton(9999);
        TEST_ASSERT_NULL(widget);  // Aucun mapping pour bouton 9999
    }
    
    static void test_finalize_positioning() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        controller.initialize();
        
        controller.finalizePositioning();
        
        // Pas de crash = succès
        TEST_ASSERT_TRUE(controller.isInitialized());
    }
    
    static void test_set_widgets_visible() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        controller.initialize();
        
        controller.setWidgetsVisible(false);
        controller.setWidgetsVisible(true);
        
        // Pas de crash = succès
        TEST_ASSERT_TRUE(controller.isInitialized());
    }
    
    static void test_event_handling_ui_parameter_update() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        controller.initialize();
        controller.setActive(true);
        
        // Créer un événement UI parameter update
        UIParameterUpdateEvent event(1, 0, 64, "Test");
        
        bool handled = controller.onEvent(event);
        
        // L'événement devrait être traité même si aucun widget n'est mappé
        TEST_ASSERT_FALSE(handled);  // Pas de widget pour CC1 dans le mock
    }
    
    static void test_event_handling_button_press() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        controller.initialize();
        controller.setActive(true);
        
        // Créer un événement bouton
        HighPriorityButtonPressEvent event(1071, true);
        
        bool handled = controller.onEvent(event);
        
        // L'événement devrait être traité
        TEST_ASSERT_TRUE(handled);
    }
    
    static void test_event_handling_while_inactive() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
        controller.initialize();
        // Ne pas activer le contrôleur
        
        UIParameterUpdateEvent event(1, 0, 64, "Test");
        bool handled = controller.onEvent(event);
        
        TEST_ASSERT_FALSE(handled);  // Pas traité car inactif
    }
    
    static void test_destructor_cleanup() {
        ParameterViewController::ControllerConfig config;
        config.enableLogging = false;
        
        {
            ParameterViewController controller(config, mockBridge, mockConfig, mockEventBus);
            controller.initialize();
            controller.setActive(true);
            TEST_ASSERT_TRUE(controller.isInitialized());
            // Le destructeur sera appelé à la fin de ce scope
        }
        
        // Si nous arrivons ici sans crash, le destructeur a fonctionné
        TEST_ASSERT_TRUE(true);
    }
};

// Définitions statiques
std::shared_ptr<MockIli9341LvglBridge> TestParameterViewController::mockBridge;
std::shared_ptr<MockUnifiedConfiguration> TestParameterViewController::mockConfig;
std::shared_ptr<MockEventBus> TestParameterViewController::mockEventBus;

// Fonctions de test Unity
void test_parameter_view_controller_constructor_default() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_constructor_default_config();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_constructor_custom() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_constructor_custom_config();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_initialize_success() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_initialize_success();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_initialize_twice() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_initialize_twice();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_initialize_missing_deps() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_initialize_missing_dependencies();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_activation() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_activation();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_update_before_init() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_update_before_init();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_update_inactive() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_update_while_inactive();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_update_active() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_update_while_active();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_set_parameter() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_set_parameter();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_set_button_state() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_set_button_state();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_get_widget_cc() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_get_widget_for_cc();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_get_widget_button() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_get_widget_for_button();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_finalize_positioning() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_finalize_positioning();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_set_visible() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_set_widgets_visible();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_event_ui_update() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_event_handling_ui_parameter_update();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_event_button() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_event_handling_button_press();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_event_inactive() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_event_handling_while_inactive();
    TestParameterViewController::tearDown();
}

void test_parameter_view_controller_destructor() {
    TestParameterViewController::setUp();
    TestParameterViewController::test_destructor_cleanup();
    TestParameterViewController::tearDown();
}