#pragma once

#include <memory>
#include <string>
#include <vector>

#include "core/domain/events/core/EventBus.hpp"
#include "config/unified/UnifiedConfiguration.hpp"

/**
 * @brief Mock simple pour Ili9341LvglBridge
 * 
 * Mock minimal pour isoler les tests d'UISubsystem
 * sans dépendre de la logique complexe LVGL.
 */
class MockIli9341LvglBridge {
public:
    MockIli9341LvglBridge() = default;
    ~MockIli9341LvglBridge() = default;

    // Mock methods pour les tests
    void refreshDisplay() {
        refresh_calls_++;
        last_refresh_time_ = millis();
    }

    bool init() {
        init_called_ = true;
        return init_should_succeed_;
    }

    // Méthodes de test
    void reset() {
        refresh_calls_ = 0;
        init_called_ = false;
        init_should_succeed_ = true;
        last_refresh_time_ = 0;
    }

    void setInitShouldSucceed(bool should_succeed) { 
        init_should_succeed_ = should_succeed; 
    }

    size_t getRefreshCallCount() const { return refresh_calls_; }
    bool wasInitCalled() const { return init_called_; }
    unsigned long getLastRefreshTime() const { return last_refresh_time_; }

private:
    size_t refresh_calls_ = 0;
    bool init_called_ = false;
    bool init_should_succeed_ = true;
    unsigned long last_refresh_time_ = 0;
};

/**
 * @brief Mock simple pour ViewManager
 * 
 * Mock pour isoler les tests de la logique de gestion des vues.
 */
class MockViewManager {
public:
    MockViewManager() = default;
    ~MockViewManager() = default;

    // Mock methods
    bool init() {
        init_called_ = true;
        return init_should_succeed_;
    }

    void update() {
        update_calls_++;
        last_update_time_ = millis();
    }

    void showMessage(const std::string& message) {
        show_message_calls_++;
        last_message_ = message;
    }

    void clearDisplay() {
        clear_display_calls_++;
    }

    // Méthodes de test
    void reset() {
        init_called_ = false;
        init_should_succeed_ = true;
        update_calls_ = 0;
        show_message_calls_ = 0;
        clear_display_calls_ = 0;
        last_message_.clear();
        last_update_time_ = 0;
    }

    void setInitShouldSucceed(bool should_succeed) { 
        init_should_succeed_ = should_succeed; 
    }

    bool wasInitCalled() const { return init_called_; }
    size_t getUpdateCallCount() const { return update_calls_; }
    size_t getShowMessageCallCount() const { return show_message_calls_; }
    size_t getClearDisplayCallCount() const { return clear_display_calls_; }
    const std::string& getLastMessage() const { return last_message_; }
    unsigned long getLastUpdateTime() const { return last_update_time_; }

private:
    bool init_called_ = false;
    bool init_should_succeed_ = true;
    size_t update_calls_ = 0;
    size_t show_message_calls_ = 0;
    size_t clear_display_calls_ = 0;
    std::string last_message_;
    unsigned long last_update_time_ = 0;
};

/**
 * @brief Mock simple pour ViewManagerEventListener
 * 
 * Mock pour l'écouteur d'événements UI.
 */
class MockViewManagerEventListener {
public:
    MockViewManagerEventListener() = default;
    ~MockViewManagerEventListener() = default;

    void subscribe() {
        subscribe_called_ = true;
    }

    void unsubscribe() {
        unsubscribe_called_ = true;
    }

    // Méthodes de test
    void reset() {
        subscribe_called_ = false;
        unsubscribe_called_ = false;
    }

    bool wasSubscribeCalled() const { return subscribe_called_; }
    bool wasUnsubscribeCalled() const { return unsubscribe_called_; }

private:
    bool subscribe_called_ = false;
    bool unsubscribe_called_ = false;
};