#include "config/ControlButtonsConfig.hpp"

const std::vector<ButtonConfig> controlButtonConfigs = {
    {.id = 51, .pin = 32, .activeLow = true, .mode = ButtonMode::TOGGLE},    // Bouton 1 en mode momentary (par défaut)
    {.id = 52, .pin = 31, .activeLow = true, .mode = ButtonMode::MOMENTARY}, // Bouton 2 en mode toggle
};
