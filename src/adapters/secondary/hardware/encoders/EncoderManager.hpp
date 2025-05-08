// adapters/secondary/hardware/encoders/EncoderManager.hpp
#pragma once
#include "core/domain/IEncoder.hpp"
#include "adapters/secondary/hardware/encoders/InterruptQuadratureEncoder.hpp"
#include "adapters/secondary/hardware/encoders/EncoderConfig.hpp"
#include <vector>
#include <memory>

/**
 * @brief Construit et expose tous les IEncoder* à partir de configs.
 */
class EncoderManager {
public:
    explicit EncoderManager(const std::vector<EncoderConfig>& cfgs);

    void updateAll();
    const std::vector<IEncoder*>& getEncoders() const;

private:
    std::vector<std::unique_ptr<IEncoder>> ownedEncoders_;
    std::vector<IEncoder*>                 encoders_;
};