#include "config/ControlEncodersConfig.hpp"

const std::vector<EncoderConfig> controlEncoderConfigs = {

    // Devices 1-8 (Mecaniques)
    {.id = 71, .pinA = 22, .pinB = 23, .ppr = 24, .hasButton = true, .pinButton = 21, .activeLowButton = true},
    {.id = 72, .pinA = 19, .pinB = 20, .ppr = 24, .hasButton = true, .pinButton = 18, .activeLowButton = true},
    {.id = 73, .pinA = 16, .pinB = 17, .ppr = 24, .hasButton = true, .pinButton = 15, .activeLowButton = true},
    {.id = 74, .pinA = 13, .pinB = 14, .ppr = 24, .hasButton = true, .pinButton = 41, .activeLowButton = true},
    {.id = 75, .pinA = 39, .pinB = 40, .ppr = 24, .hasButton = true, .pinButton = 38, .activeLowButton = true},
    {.id = 76, .pinA = 36, .pinB = 37, .ppr = 24, .hasButton = true, .pinButton = 35, .activeLowButton = true},
    {.id = 77, .pinA = 33, .pinB = 34, .ppr = 24, .hasButton = true, .pinButton = 30, .activeLowButton = true},
    {.id = 78, .pinA = 28, .pinB = 29, .ppr = 24, .hasButton = true, .pinButton = 27, .activeLowButton = true},

    // Navigation
    {.id = 79, .pinA = 9, .pinB = 10, .ppr = 96, .hasButton = true, .pinButton = 8, .activeLowButton = true},

    // Optique
    {.id = 80, .pinA = 11, .pinB = 12, .ppr = 600, .hasButton = false, .activeLowButton = true}};
