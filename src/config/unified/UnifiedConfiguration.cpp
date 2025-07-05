#include "config/unified/UnifiedConfiguration.hpp"

#include <algorithm>
#include <unordered_set>

UnifiedConfiguration::UnifiedConfiguration() {
    // === OPTIMISATIONS MÉMOIRE TEENSY 4.1 ===
    controls_.reserve(20);  // Pré-allocation pour éviter réallocations
    idIndex_.reserve(25);   // IDs + boutons d'encodeurs
    // Utilisation estimée: ~3-4KB au lieu de 8-12KB avec réallocations
}

void UnifiedConfiguration::addControl(ControlDefinition control) {
    // Vérifier l'unicité de l'ID
    if (idIndex_.find(control.id) != idIndex_.end()) {
        // Duplicate control ID - ignore in embedded environment
        return;
    }

    size_t index = controls_.size();
    controls_.push_back(std::move(control));
    idIndex_[controls_.back().id] = index;

    // Si c'est un encodeur avec bouton, indexer aussi l'ID du bouton
    if (controls_.back().hardware.type == InputType::ENCODER &&
        controls_.back().hardware.encoderButtonPin) {
        InputId buttonId = controls_.back().getEncoderButtonId();
        if (buttonId > 0) {
            idIndex_[buttonId] = index;
        }
    }
}

const std::vector<ControlDefinition>& UnifiedConfiguration::getAllControls() const {
    return controls_;
}

std::optional<ControlDefinition> UnifiedConfiguration::findControlById(InputId id) const {
    auto it = idIndex_.find(id);
    if (it != idIndex_.end()) {
        return controls_[it->second];
    }
    return std::nullopt;
}

std::vector<ControlDefinition> UnifiedConfiguration::getControlsByRole(MappingRole role) const {
    std::vector<ControlDefinition> result;

    for (const auto& control : controls_) {
        if (control.hasRole(role)) {
            result.push_back(control);
        }
    }

    return result;
}

Result<void> UnifiedConfiguration::validate() const {
    // Vérifier l'unicité des IDs
    std::unordered_set<InputId> seenIds;

    for (const auto& control : controls_) {
        // Vérifier l'ID principal
        if (control.id == 0) {
            return Result<void>::error({ErrorCode::ConfigurationError, "Control ID cannot be 0"});
        }

        if (!seenIds.insert(control.id).second) {
            return Result<void>::error({ErrorCode::ConfigurationError, "Duplicate control ID found"});
        }

        // Vérifier l'ID du bouton d'encodeur
        if (control.hardware.type == InputType::ENCODER && control.hardware.encoderButtonPin) {
            InputId buttonId = control.getEncoderButtonId();
            if (!seenIds.insert(buttonId).second) {
                return Result<void>::error({ErrorCode::ConfigurationError, "Duplicate encoder button ID found"});
            }
        }

        // Vérifier que chaque mapping a un rôle valide
        for (const auto& mapping : control.mappings) {
            // MappingRole n'a pas de valeur NONE, on vérifie juste la cohérence
            if (mapping.appliesTo == MappingControlType::ENCODER &&
                control.hardware.type != InputType::ENCODER) {
                return Result<void>::error({ErrorCode::ConfigurationError, "Encoder mapping on non-encoder control"});
            }
        }
    }

    return Result<void>::success();
}

UnifiedConfiguration::Stats UnifiedConfiguration::getStats() const {
    Stats stats{};
    stats.totalControls = controls_.size();

    for (const auto& control : controls_) {
        if (control.hardware.type == InputType::ENCODER) {
            stats.encoders++;
            if (control.hardware.encoderButtonPin) {
                stats.buttons++;  // Bouton de l'encodeur
            }
        } else if (control.hardware.type == InputType::BUTTON) {
            stats.buttons++;
        }

        for (const auto& mapping : control.mappings) {
            if (mapping.role == MappingRole::MIDI) {
                stats.midiMappings++;
            } else if (mapping.role == MappingRole::NAVIGATION) {
                stats.navigationMappings++;
            }
        }
    }

    return stats;
}
