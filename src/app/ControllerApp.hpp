// app/ControllerApp.hpp
#pragma once

#include <Arduino.h>
#include <vector>

#include "interface_adapters/encoders/EncoderManager.hpp"
#include "use_cases/ProcessEncoders.hpp"
#include "interface_adapters/buttons/DigitalButtonManager.hpp"
#include "use_cases/ProcessButtons.hpp"

// Config externes
#include "config/ControlEncodersConfig.hpp"
#include "config/ControlButtonsConfig.hpp"


/**
 * @brief Application principale du contrôleur MIDI
 */
class ControllerApp {
public:
    ControllerApp();

    /// alias pour setup()
    void init()   { begin(); }
    /// alias pour loop()
    void update() { tick(); }

    void begin();
    void tick();

private:
    EncoderManager       encoderManager_;
    ProcessEncoders      processEncoders_;

    DigitalButtonManager buttonManager_;
    ProcessButtons       processButtons_;
};
