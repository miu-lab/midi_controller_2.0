// interface_adapters/buttons/DigitalButtonManager.hpp
#pragma once

#include "interface_adapters/buttons/DigitalButtonConfig.hpp"
#include "interface_adapters/buttons/DigitalButton.hpp"
#include "domain/IButton.hpp"
#include <vector>
#include <memory>

/**
 * @brief Manager pour plusieurs Button configurés dynamiquement.
 */
class DigitalButtonManager {
public:
    explicit DigitalButtonManager(const std::vector<ButtonConfig>& configs);
    ~DigitalButtonManager();

    void updateAll();
    const std::vector<IButton*>& getButtons() const;

private:
    std::vector<std::unique_ptr<DigitalButton>> ownedButtons_;  // possession des boutons
    std::vector<IButton*>                       buttons_;       // pointeurs pour use-cases
};