#pragma once

#include "adapters/primary/ui/ViewManager.hpp"

class ViewManagerMock : public ViewManager {
public:
    explicit ViewManagerMock(DisplayPort& display) : display_(display) {}

    // Implémentation des méthodes virtuelles pures
    void enterMenu() override {}

    void exitMenu() override {}

    void selectNextMenuItem() override {}

    void selectPreviousMenuItem() override {}

    void selectMenuItem(int index) override {}

    int getCurrentMenuIndex() const override {
        return 0;
    }

    bool isInMenu() const override {
        return false;
    }

    void showMainScreen() override {}

    void showControlMonitor() override {}

    void showDebugScreen() override {}

    void showModalDialog(const String& message) override {}

    void hideModalDialog() override {}

    void toggleModalDialogButton() override {}

    bool isModalDialogOkSelected() const override {
        return true;
    }

    void scrollMainScreenByDelta(int8_t delta) override {}

    void scrollControlMonitorByDelta(int8_t delta) override {}

    void scrollDebugLogByDelta(int8_t delta) override {}

private:
    DisplayPort& display_;
};
