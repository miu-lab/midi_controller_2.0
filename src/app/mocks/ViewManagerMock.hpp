#pragma once

#include "adapters/primary/ui/ViewManager.hpp"

class ViewManagerMock : public ViewManager {
public:
    explicit ViewManagerMock(DisplayPort& display) : display_(display) {}

    // Nouvelles méthodes ajoutées
    bool init() override {
        return true; // Mock: toujours réussi
    }
    
    void update() override {
        // Mock: ne fait rien
    }
    
    void render() override {
        // Mock: ne fait rien
    }

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

    // Implémentation des méthodes pour la mise à jour de l'interface
    void updateControlMonitorInfo(uint8_t controlId, const String& type, 
                                uint8_t channel, uint8_t number, uint8_t value) override {
        // Mock: ne fait rien
    }

    void updateEncoderPosition(uint8_t encoderId, int32_t position) override {
        // Mock: ne fait rien
    }

    void updateEncoderButtonState(uint8_t encoderId, bool pressed) override {
        // Mock: ne fait rien
    }

    void updateButtonState(uint8_t buttonId, bool pressed) override {
        // Mock: ne fait rien
    }

private:
    DisplayPort& display_;
};
