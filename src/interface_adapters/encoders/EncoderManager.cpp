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
        // Ne PAS appeler readDelta() ici car cela consommerait les deltas
        // avant que ProcessEncoders::update() ne puisse les traiter
        // e->readDelta(); // Commenté pour éviter la consommation des deltas
        
        // On vérifie quand même l'état des boutons d'encodeur
        (void)e->isPressed();
    }
}

const std::vector<IEncoder*>& EncoderManager::getEncoders() const {
    return encoders_;
}
