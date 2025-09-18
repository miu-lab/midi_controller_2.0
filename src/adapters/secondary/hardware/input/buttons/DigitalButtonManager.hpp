// adapters/secondary/hardware/input/buttons/DigitalButtonManager.hpp
#pragma once

#include <memory>
#include <vector>

#include "adapters/secondary/hardware/input/buttons/ButtonConfig.hpp"
#include "adapters/secondary/hardware/input/buttons/UnifiedButton.hpp"
#include "core/ports/input/ButtonPort.hpp"

/**
 * @brief Manager pour plusieurs Button configurés dynamiquement.
 */
class DigitalButtonManager {
public:
    explicit DigitalButtonManager(const std::vector<ButtonConfig>& configs);
    ~DigitalButtonManager();

    // Désactiver la copie
    DigitalButtonManager(const DigitalButtonManager&) = delete;
    DigitalButtonManager& operator=(const DigitalButtonManager&) = delete;

    // Permettre le déplacement si nécessaire
    DigitalButtonManager(DigitalButtonManager&&) = default;
    DigitalButtonManager& operator=(DigitalButtonManager&&) = default;

    void updateAll();
    const std::vector<ButtonPort*>& getButtons() const;

    // Nouvelles méthodes pour contrôler les boutons
    void resetAllToggleStates();               // Réinitialiser tous les boutons toggle
    void resetToggleState(ButtonId buttonId);  // Réinitialiser un bouton toggle spécifique

private:
    std::vector<std::unique_ptr<UnifiedButton>> ownedButtons_;  // possession des boutons unifiés
    std::vector<ButtonPort*> buttons_;                          // pointeurs pour use-cases
};