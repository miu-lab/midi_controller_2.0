#include "DisplayManagerAdapter.hpp"
#include <Arduino.h>
#include "config/PerformanceConfig.hpp"

DisplayManagerAdapter::DisplayManagerAdapter(std::shared_ptr<Ili9341LvglBridge> lvglBridge)
    : lvglBridge_(lvglBridge)
    , refreshIntervalMs_(PerformanceConfig::DISPLAY_REFRESH_PERIOD_MS * PerformanceConfig::VSYNC_SPACING)
    , lastRefreshTime_(0) {
}

void DisplayManagerAdapter::update() {
    if (!lvglBridge_) {
        return;
    }
    
    unsigned long currentTime = getCurrentTime();
    
    if ((currentTime - lastRefreshTime_) >= refreshIntervalMs_) {
        lvglBridge_->refreshDisplay();
        lastRefreshTime_ = currentTime;
    }
}

void DisplayManagerAdapter::forceRefresh() {
    if (!lvglBridge_) {
        return;
    }
    
    lvglBridge_->refreshDisplay();
    lastRefreshTime_ = getCurrentTime();
}

void DisplayManagerAdapter::setRefreshInterval(unsigned long intervalMs) {
    refreshIntervalMs_ = intervalMs;
}

unsigned long DisplayManagerAdapter::getRefreshInterval() const {
    return refreshIntervalMs_;
}

unsigned long DisplayManagerAdapter::getCurrentTime() const {
    return millis();
}