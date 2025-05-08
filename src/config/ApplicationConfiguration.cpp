#include "config/ApplicationConfiguration.hpp"

ApplicationConfiguration::ApplicationConfiguration()
    : hardwareConfiguration(), mappingConfiguration(), encoderSensitivity_(1.0f) {
    // Tout est initialisé par les constructeurs des membres
}

const HardwareConfiguration& ApplicationConfiguration::getHardwareConfiguration() const {
    return hardwareConfiguration;
}

const MappingConfiguration& ApplicationConfiguration::getMappingConfiguration() const {
    return mappingConfiguration;
}

void ApplicationConfiguration::setEncoderSensitivity(float value) {
    encoderSensitivity_ = value;
}

float ApplicationConfiguration::getEncoderSensitivity() const {
    return encoderSensitivity_;
}
