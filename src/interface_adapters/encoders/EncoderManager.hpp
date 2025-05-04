// interface_adapters/encoders/EncoderManager.hpp
#pragma once
#include "domain/IEncoder.hpp"
#include "interface_adapters/encoders/InterruptQuadratureEncoder.hpp"
#include "interface_adapters/encoders/EncoderConfig.hpp"
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