// interface_adapters/encoders/EncoderManager.cpp
#include "interface_adapters/encoders/EncoderManager.hpp"

EncoderManager::EncoderManager(const std::vector<EncoderConfig>& cfgs) {
    for (auto& cfg : cfgs) {
        ownedEncoders_.push_back(
            std::make_unique<InterruptQuadratureEncoder>(cfg)
        );
    }
    for (auto& up : ownedEncoders_) {
        encoders_.push_back(up.get());
    }
}

void EncoderManager::updateAll() {
    for (auto* e : encoders_) {
        e->readDelta();
        (void)e->isPressed();
    }
}

const std::vector<IEncoder*>& EncoderManager::getEncoders() const {
    return encoders_;
}
