// adapters/secondary/hardware/input/encoders/EncoderManager.cpp
#include "adapters/secondary/hardware/input/encoders/EncoderManager.hpp"

EncoderManager::EncoderManager(const std::vector<EncoderConfig>& cfgs) {
    for (auto& cfg : cfgs) {
        ownedEncoders_.push_back(std::make_unique<InterruptQuadratureEncoder>(cfg));
    }
    for (auto& up : ownedEncoders_) {
        encoders_.push_back(up.get());
    }
}

void EncoderManager::updateAll() {
    // Les encodeurs sont maintenant purs (sans boutons intégrés)
    // Aucune mise à jour nécessaire ici - les deltas sont lus par ProcessEncoders
}

const std::vector<EncoderPort*>& EncoderManager::getEncoders() const {
    return encoders_;
}
