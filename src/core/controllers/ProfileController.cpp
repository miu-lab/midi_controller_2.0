#include "core/controllers/ProfileController.hpp"

ProfileController::ProfileController(ProfileStoragePort& profileManager)
    : profileManager_(profileManager) {}

std::vector<ControlDefinition> ProfileController::getAllControlDefinitions() const {
    return profileManager_.getAllControlDefinitions();
}

std::optional<ControlDefinition> ProfileController::getControlDefinition(InputId controlId) const {
    return profileManager_.getControlDefinition(controlId);
}

void ProfileController::setControlDefinition(const ControlDefinition& controlDef) {
    profileManager_.setControlDefinition(controlDef);
}

bool ProfileController::removeMapping(InputId controlId) {
    return profileManager_.removeBinding(controlId);
}

void ProfileController::resetToDefaults() {
    profileManager_.resetToDefaults();
}

bool ProfileController::saveProfile() {
    return profileManager_.saveProfile();
}

bool ProfileController::loadProfile() {
    return profileManager_.loadProfile();
}

std::unique_ptr<IMidiMappingStrategy> ProfileController::createMappingStrategy(
    InputId controlId, const ControlDefinition& controlDef) const {
    
    // Chercher le premier mapping MIDI dans la définition
    for (const auto& mappingSpec : controlDef.mappings) {
        if (mappingSpec.role == MappingRole::MIDI) {
            const auto& midiConfig = std::get<ControlDefinition::MidiConfig>(mappingSpec.config);
            
            // Déterminer si c'est un bouton ou un encodeur
            bool isButton = (controlDef.hardware.type == InputType::BUTTON) ||
                           (mappingSpec.appliesTo == MappingControlType::BUTTON);

            if (midiConfig.isRelative && !isButton) {
                // Mode relatif pour les encodeurs
                return MidiMappingFactory::createRelative();
            } else {
                // Mode absolu pour les boutons ou encodeurs absolus
                return MidiMappingFactory::createAbsolute(0, 127);
            }
        }
    }
    
    // Par défaut, retourner une stratégie relative
    return MidiMappingFactory::createRelative();
}
