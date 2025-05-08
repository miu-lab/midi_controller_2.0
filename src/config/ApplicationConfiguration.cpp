#include "config/ApplicationConfiguration.hpp"

ApplicationConfiguration::ApplicationConfiguration()
    : hardwareConfiguration(), mappingConfiguration() {
    // Tout est initialisé par les constructeurs des membres
}

const HardwareConfiguration& ApplicationConfiguration::getHardwareConfiguration() const {
    return hardwareConfiguration;
}

const MappingConfiguration& ApplicationConfiguration::getMappingConfiguration() const {
    return mappingConfiguration;
}
