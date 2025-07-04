#include "DisplayManager.hpp"
#include <Arduino.h>
#include "config/PerformanceConfig.hpp"

DisplayManager::DisplayManager(std::shared_ptr<Ili9341LvglBridge> lvglBridge)
    : lvglBridge_(lvglBridge)
    , refreshIntervalMs_(PerformanceConfig::DISPLAY_REFRESH_PERIOD_MS * PerformanceConfig::VSYNC_SPACING)
    , lastRefreshTime_(0) {
}

void DisplayManager::update() {
    if (!lvglBridge_) {
        return;
    }
    
    unsigned long currentTime = getCurrentTime();
    
    if ((currentTime - lastRefreshTime_) >= refreshIntervalMs_) {
        lvglBridge_->refreshDisplay();
        lastRefreshTime_ = currentTime;
    }
}

void DisplayManager::forceRefresh() {
    if (!lvglBridge_) {
        return;
    }
    
    lvglBridge_->refreshDisplay();
    lastRefreshTime_ = getCurrentTime();
}

void DisplayManager::setRefreshInterval(unsigned long intervalMs) {
    refreshIntervalMs_ = intervalMs;
}

unsigned long DisplayManager::getRefreshInterval() const {
    return refreshIntervalMs_;
}

unsigned long DisplayManager::getCurrentTime() const {
    return millis();
}