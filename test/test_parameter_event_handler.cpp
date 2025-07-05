#include <unity.h>
#include "adapters/primary/ui/parameter/ParameterEventHandler.hpp"
#include "adapters/ui/lvgl/widgets/ParameterWidget.hpp"
#include "core/domain/events/UIEvent.hpp"
#include "core/domain/events/MidiEvents.hpp"

// Mock ParameterWidget pour les tests
class MockParameterWidget : public ParameterWidget {
public:
    MockParameterWidget() : ParameterWidget(nullptr, 80, 120, 70) {
        hasButtonIndicator_ = false;
        lastCC_ = 0;
        lastChannel_ = 0;
        lastValue_ = 0;
        lastButtonState_ = false;
        setParameterCalled_ = false;
        setButtonStateCalled_ = false;
    }
    
    void setParameter(uint8_t cc_number, uint8_t channel, uint8_t value, 
                     const String& parameter_name, bool animate = true) override {
        lastCC_ = cc_number;
        lastChannel_ = channel;
        lastValue_ = value;
        lastParameterName_ = parameter_name;
        lastAnimate_ = animate;
        setParameterCalled_ = true;
    }
    
    void setButtonState(bool pressed, bool animate = true) override {
        lastButtonState_ = pressed;
        lastAnimate_ = animate;
        setButtonStateCalled_ = true;
    }
    
    bool hasButtonIndicator() const override {
        return hasButtonIndicator_;
    }
    
    void setHasButtonIndicator(bool has) { hasButtonIndicator_ = has; }
    
    // Getters pour les tests
    uint8_t getLastCC() const { return lastCC_; }
    uint8_t getLastChannel() const { return lastChannel_; }
    uint8_t getLastValue() const { return lastValue_; }
    String getLastParameterName() const { return lastParameterName_; }
    bool getLastButtonState() const { return lastButtonState_; }
    bool getLastAnimate() const { return lastAnimate_; }
    bool wasSetParameterCalled() const { return setParameterCalled_; }
    bool wasSetButtonStateCalled() const { return setButtonStateCalled_; }
    
    void resetCallFlags() {
        setParameterCalled_ = false;
        setButtonStateCalled_ = false;
    }

private:
    bool hasButtonIndicator_;
    uint8_t lastCC_, lastChannel_, lastValue_;
    String lastParameterName_;
    bool lastButtonState_, lastAnimate_;
    bool setParameterCalled_, setButtonStateCalled_;
};

class TestParameterEventHandler {
public:
    static MockParameterWidget* mockWidgets[8];
    static std::shared_ptr<ParameterWidgetMappingManager> mockMappingManager;

    static void setUp() {
        // Créer les mock widgets
        for (int i = 0; i < 8; i++) {
            mockWidgets[i] = new MockParameterWidget();
        }
        
        // Créer un mapping manager configuré
        mockMappingManager = std::make_shared<ParameterWidgetMappingManager>();

        // Configuration de test avec CC1-3 mappés aux widgets 0-2
        std::vector<ConfigurationMidiExtractor::MidiControlInfo> midiControls;
        for (int i = 0; i < 3; i++) {
            ConfigurationMidiExtractor::MidiControlInfo info;
            info.cc_number = i + 1;  // CC1, CC2, CC3
            info.channel = 0;
            info.name = "ENC" + String(i + 1);
            info.control_id = 71 + i;
            midiControls.push_back(info);
        }
        
        // Bouton test mappé au widget 0
        std::vector<ConfigurationMidiExtractor::ButtonInfo> buttonInfos;
        ConfigurationMidiExtractor::ButtonInfo buttonInfo;
        buttonInfo.button_id = 1071;
        buttonInfo.parent_encoder_id = 71;  // Parent de CC1
        buttonInfo.name = "ENC1 BTN";
        buttonInfos.push_back(buttonInfo);
        
        mockMappingManager->initializeMappings(midiControls, buttonInfos);
    }
    
    static void tearDown() {
        for (int i = 0; i < 8; i++) {
            delete mockWidgets[i];
            mockWidgets[i] = nullptr;
        }
        mockMappingManager.reset();
    }
    
    static ParameterWidget* widgetAccessor(uint8_t index) {
        if (index < 8) {
            return mockWidgets[index];
        }
        return nullptr;
    }

    static void test_constructor_default_config() {
        ParameterEventHandler::EventConfig config;
        ParameterEventHandler handler(config, widgetAccessor, mockMappingManager);
        
        TEST_ASSERT_TRUE(handler.isActive());
        
        auto stats = handler.getStats();
        TEST_ASSERT_EQUAL(0, stats.totalEvents);
    }

    static void test_constructor_custom_config() {
        ParameterEventHandler::EventConfig config;
        config.enableMidiEvents = false;
        config.enableButtonEvents = true;
        config.enableLogging = true;
        config.enableAnimation = false;
        
        ParameterEventHandler handler(config, widgetAccessor, mockMappingManager);
        
        TEST_ASSERT_TRUE(handler.isActive());
    }

    static void test_handle_midi_parameter_update_event() {
        ParameterEventHandler::EventConfig config;
        ParameterEventHandler handler(config, widgetAccessor, mockMappingManager);
        
        // Créer un événement de mise à jour MIDI
        UIParameterUpdateEvent midiEvent(1, 0, 64, "Test Param");  // CC1, canal 0, valeur 64
        
        // Traiter l'événement
        bool handled = handler.onEvent(midiEvent);
        
        TEST_ASSERT_TRUE(handled);
        
        // Vérifier que le widget a été mis à jour
        MockParameterWidget* widget = mockWidgets[0];
        TEST_ASSERT_TRUE(widget->wasSetParameterCalled());
        TEST_ASSERT_EQUAL(1, widget->getLastCC());
        TEST_ASSERT_EQUAL(1, widget->getLastChannel());  // Converti en 1-based
        TEST_ASSERT_EQUAL(64, widget->getLastValue());
        TEST_ASSERT_EQUAL_STRING("Test Param", widget->getLastParameterName().c_str());
        
        // Vérifier les statistiques
        auto stats = handler.getStats();
        TEST_ASSERT_EQUAL(1, stats.totalEvents);
        TEST_ASSERT_EQUAL(1, stats.midiEventsProcessed);
    }

    static void test_handle_midi_event_unmapped_cc() {
        ParameterEventHandler::EventConfig config;
        ParameterEventHandler handler(config, widgetAccessor, mockMappingManager);
        
        // Créer un événement pour un CC non mappé
        UIParameterUpdateEvent midiEvent(99, 0, 64);    // CC99 non mappé
        
        bool handled = handler.onEvent(midiEvent);
        
        TEST_ASSERT_FALSE(handled);
        
        // Aucun widget ne doit avoir été appelé
        for (int i = 0; i < 8; i++) {
            TEST_ASSERT_FALSE(mockWidgets[i]->wasSetParameterCalled());
        }
        
        auto stats = handler.getStats();
        TEST_ASSERT_EQUAL(1, stats.totalEvents);
        TEST_ASSERT_EQUAL(0, stats.midiEventsProcessed);
    }

    static void test_handle_button_event() {
        ParameterEventHandler::EventConfig config;
        ParameterEventHandler handler(config, widgetAccessor, mockMappingManager);
        
        // Configurer le widget 0 pour avoir un bouton indicateur
        mockWidgets[0]->setHasButtonIndicator(true);
        
        // Créer un événement de bouton
        HighPriorityButtonPressEvent buttonEvent(1071, true);  // Mappé au widget 0
        
        bool handled = handler.onEvent(buttonEvent);
        
        // L'événement est traité mais pas marqué comme "handled" (return false)
        TEST_ASSERT_FALSE(handled);
        
        // Vérifier que le widget a été mis à jour
        MockParameterWidget* widget = mockWidgets[0];
        TEST_ASSERT_TRUE(widget->wasSetButtonStateCalled());
        TEST_ASSERT_TRUE(widget->getLastButtonState());
        
        auto stats = handler.getStats();
        TEST_ASSERT_EQUAL(1, stats.totalEvents);
        TEST_ASSERT_EQUAL(1, stats.buttonEventsProcessed);
    }

    static void test_handle_button_event_no_indicator() {
        ParameterEventHandler::EventConfig config;
        ParameterEventHandler handler(config, widgetAccessor, mockMappingManager);
        
        // Widget 0 sans indicateur de bouton
        mockWidgets[0]->setHasButtonIndicator(false);
        
        HighPriorityButtonPressEvent buttonEvent(1071, true);
        
        bool handled = handler.onEvent(buttonEvent);
        
        TEST_ASSERT_FALSE(handled);
        TEST_ASSERT_FALSE(mockWidgets[0]->wasSetButtonStateCalled());
        
        auto stats = handler.getStats();
        TEST_ASSERT_EQUAL(1, stats.totalEvents);
        TEST_ASSERT_EQUAL(0, stats.buttonEventsProcessed);
    }

    static void test_setActive_functionality() {
        ParameterEventHandler::EventConfig config;
        ParameterEventHandler handler(config, widgetAccessor, mockMappingManager);
        
        TEST_ASSERT_TRUE(handler.isActive());
        
        // Désactiver
        handler.setActive(false);
        TEST_ASSERT_FALSE(handler.isActive());
        
        // Tester qu'un événement est ignoré quand inactif
        UIParameterUpdateEvent midiEvent(1, 0, 64);
        
        bool handled = handler.onEvent(midiEvent);
        TEST_ASSERT_FALSE(handled);
        
        auto stats = handler.getStats();
        TEST_ASSERT_EQUAL(1, stats.eventsIgnored);
        TEST_ASSERT_EQUAL(0, stats.midiEventsProcessed);
    }

    static void test_midi_events_disabled() {
        ParameterEventHandler::EventConfig config;
        config.enableMidiEvents = false;  // Désactiver les événements MIDI
        ParameterEventHandler handler(config, widgetAccessor, mockMappingManager);
        
        UIParameterUpdateEvent midiEvent(1, 0, 64);
        
        bool handled = handler.onEvent(midiEvent);
        TEST_ASSERT_FALSE(handled);
        
        auto stats = handler.getStats();
        TEST_ASSERT_EQUAL(1, stats.eventsIgnored);
        TEST_ASSERT_EQUAL(0, stats.midiEventsProcessed);
    }

    static void test_button_events_disabled() {
        ParameterEventHandler::EventConfig config;
        config.enableButtonEvents = false;  // Désactiver les événements de boutons
        ParameterEventHandler handler(config, widgetAccessor, mockMappingManager);
        
        mockWidgets[0]->setHasButtonIndicator(true);
        
        HighPriorityButtonPressEvent buttonEvent(1071, true);
        
        bool handled = handler.onEvent(buttonEvent);
        TEST_ASSERT_FALSE(handled);
        
        auto stats = handler.getStats();
        TEST_ASSERT_EQUAL(1, stats.eventsIgnored);
        TEST_ASSERT_EQUAL(0, stats.buttonEventsProcessed);
    }

    static void test_update_config() {
        ParameterEventHandler::EventConfig config;
        ParameterEventHandler handler(config, widgetAccessor, mockMappingManager);
        
        // Changer la configuration
        ParameterEventHandler::EventConfig newConfig;
        newConfig.enableAnimation = false;
        handler.updateConfig(newConfig);
        
        // Tester qu'animation est désactivée
        UIParameterUpdateEvent midiEvent(1, 0, 64);
        
        handler.onEvent(midiEvent);
        
        TEST_ASSERT_FALSE(mockWidgets[0]->getLastAnimate());
    }

    static void test_stats_functionality() {
        ParameterEventHandler::EventConfig config;
        ParameterEventHandler handler(config, widgetAccessor, mockMappingManager);
        
        mockWidgets[0]->setHasButtonIndicator(true);
        
        // Traiter différents types d'événements
        UIParameterUpdateEvent midiEvent(1, 0, 64);
        handler.onEvent(midiEvent);
        
        HighPriorityButtonPressEvent buttonEvent(1071, true);
        handler.onEvent(buttonEvent);
        
        // Événement non géré
        UIParameterUpdateEvent unmappedEvent(99, 0, 64);  // Non mappé
        handler.onEvent(unmappedEvent);
        
        auto stats = handler.getStats();
        TEST_ASSERT_EQUAL(3, stats.totalEvents);
        TEST_ASSERT_EQUAL(1, stats.midiEventsProcessed);
        TEST_ASSERT_EQUAL(1, stats.buttonEventsProcessed);
        
        // Reset des stats
        handler.resetStats();
        auto resetStats = handler.getStats();
        TEST_ASSERT_EQUAL(0, resetStats.totalEvents);
    }

    static void test_empty_parameter_name() {
        ParameterEventHandler::EventConfig config;
        ParameterEventHandler handler(config, widgetAccessor, mockMappingManager);
        
        UIParameterUpdateEvent midiEvent(1, 0, 64, "");  // Nom vide
        
        handler.onEvent(midiEvent);
        
        // Doit générer un nom par défaut "CC1"
        TEST_ASSERT_EQUAL_STRING("CC1", mockWidgets[0]->getLastParameterName().c_str());
    }

    static void test_null_widget_accessor() {
        ParameterEventHandler::EventConfig config;
        ParameterEventHandler handler(config, nullptr, mockMappingManager);
        
        UIParameterUpdateEvent midiEvent(1, 0, 64);
        
        bool handled = handler.onEvent(midiEvent);
        TEST_ASSERT_FALSE(handled);
    }

    static void test_null_mapping_manager() {
        ParameterEventHandler::EventConfig config;
        ParameterEventHandler handler(config, widgetAccessor, nullptr);
        
        UIParameterUpdateEvent midiEvent(1, 0, 64);
        
        bool handled = handler.onEvent(midiEvent);
        TEST_ASSERT_FALSE(handled);
    }
};

// Définitions statiques
MockParameterWidget* TestParameterEventHandler::mockWidgets[8] = {nullptr};
std::shared_ptr<ParameterWidgetMappingManager> TestParameterEventHandler::mockMappingManager;

void test_parameter_event_handler_constructor_default() {
    TestParameterEventHandler::setUp();
    TestParameterEventHandler::test_constructor_default_config();
    TestParameterEventHandler::tearDown();
}

void test_parameter_event_handler_constructor_custom() {
    TestParameterEventHandler::setUp();
    TestParameterEventHandler::test_constructor_custom_config();
    TestParameterEventHandler::tearDown();
}

void test_parameter_event_handler_midi_event() {
    TestParameterEventHandler::setUp();
    TestParameterEventHandler::test_handle_midi_parameter_update_event();
    TestParameterEventHandler::tearDown();
}

void test_parameter_event_handler_unmapped_cc() {
    TestParameterEventHandler::setUp();
    TestParameterEventHandler::test_handle_midi_event_unmapped_cc();
    TestParameterEventHandler::tearDown();
}

void test_parameter_event_handler_button_event() {
    TestParameterEventHandler::setUp();
    TestParameterEventHandler::test_handle_button_event();
    TestParameterEventHandler::tearDown();
}

void test_parameter_event_handler_button_no_indicator() {
    TestParameterEventHandler::setUp();
    TestParameterEventHandler::test_handle_button_event_no_indicator();
    TestParameterEventHandler::tearDown();
}

void test_parameter_event_handler_active_functionality() {
    TestParameterEventHandler::setUp();
    TestParameterEventHandler::test_setActive_functionality();
    TestParameterEventHandler::tearDown();
}

void test_parameter_event_handler_midi_disabled() {
    TestParameterEventHandler::setUp();
    TestParameterEventHandler::test_midi_events_disabled();
    TestParameterEventHandler::tearDown();
}

void test_parameter_event_handler_button_disabled() {
    TestParameterEventHandler::setUp();
    TestParameterEventHandler::test_button_events_disabled();
    TestParameterEventHandler::tearDown();
}

void test_parameter_event_handler_update_config() {
    TestParameterEventHandler::setUp();
    TestParameterEventHandler::test_update_config();
    TestParameterEventHandler::tearDown();
}

void test_parameter_event_handler_stats() {
    TestParameterEventHandler::setUp();
    TestParameterEventHandler::test_stats_functionality();
    TestParameterEventHandler::tearDown();
}

void test_parameter_event_handler_empty_name() {
    TestParameterEventHandler::setUp();
    TestParameterEventHandler::test_empty_parameter_name();
    TestParameterEventHandler::tearDown();
}

void test_parameter_event_handler_null_accessor() {
    TestParameterEventHandler::setUp();
    TestParameterEventHandler::test_null_widget_accessor();
    TestParameterEventHandler::tearDown();
}

void test_parameter_event_handler_null_manager() {
    TestParameterEventHandler::setUp();
    TestParameterEventHandler::test_null_mapping_manager();
    TestParameterEventHandler::tearDown();
}