#include "ProfileController.hpp"

ProfileController::ProfileController(ProfileStoragePort& profileManager)
    : profileManager_(profileManager) {}

std::vector<InputMapping> ProfileController::getAllMappings() const {
    return profileManager_.getAllMappings();
}

MidiControl ProfileController::getMapping(InputId controlId) const {
    auto mapping = profileManager_.getBinding(controlId);
    if (mapping) {
        return *mapping;
    }

    // Valeur par défaut si le mapping n'existe pas
    return {.channel = 0, .control = 0, .isRelative = false};
}

void ProfileController::setMapping(InputId controlId, const MidiControl& midiControl) {
    profileManager_.setBinding(controlId, midiControl);
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
    InputId controlId, const MidiControl& midiControl) const {
    // Déterminer si c'est un encodeur ou un bouton
    bool isEncoder =
        controlId >= 70 && controlId < 90;  // Supposons que les IDs 70-89 sont des encodeurs
    bool isButton =
        controlId >= 50 && controlId < 70;  // Supposons que les IDs 50-69 sont des boutons

    if (midiControl.isRelative) {
        // Mode relatif - bien pour les encodeurs
        return MidiMappingFactory::createRelative();
    } else {
        // Mode absolu
        if (isEncoder) {
            // Pour les encodeurs en mode absolu, utiliser une plage dynamique
            // qui s'adapte à la position actuelle de l'encodeur
            return MidiMappingFactory::createDynamicRange(0, 127);
        } else if (isButton) {
            // Pour les boutons, c'est toujours 0 ou 127
            return MidiMappingFactory::createAbsolute(0, 1);
        } else {
            // Par défaut
            return MidiMappingFactory::createAbsolute(0, 127);
        }
    }
}
