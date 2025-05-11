// adapters/secondary/hardware/encoders/EncoderManager.hpp
#pragma once
#include <memory>
#include <vector>

#include "adapters/secondary/hardware/encoders/EncoderConfig.hpp"
#include "adapters/secondary/hardware/encoders/InterruptQuadratureEncoder.hpp"
#include "core/ports/input/EncoderPort.hpp"

/**
 * @brief Construit et expose tous les EncoderPort* à partir de configs.
 */
class EncoderManager {
public:
    explicit EncoderManager(const std::vector<EncoderConfig>& cfgs);

    void updateAll();
    const std::vector<EncoderPort*>& getEncoders() const;

private:
    std::vector<std::unique_ptr<EncoderPort>> ownedEncoders_;
    std::vector<EncoderPort*> encoders_;
};