#pragma once

#include <memory>
#include "adapters/secondary/hardware/input/buttons/ButtonConfig.hpp"
#include "adapters/secondary/hardware/input/buttons/readers/IPinReader.hpp"
#include "core/ports/input/ButtonPort.hpp"

/**
 * @brief Bouton unifié utilisant le Strategy Pattern pour la lecture de pin
 *
 * Cette classe centralise toute la logique de debounce, toggle et état
 * en déléguant seulement la lecture de pin à une stratégie configurable.
 */
class UnifiedButton : public ButtonPort {
public:
    /**
     * @brief Constructeur avec configuration et stratégie de lecture
     * @param cfg Configuration du bouton
     * @param pinReader Stratégie de lecture de pin (ownership transféré)
     */
    UnifiedButton(const ButtonConfig& cfg, std::unique_ptr<IPinReader> pinReader);

    // Interface ButtonPort
    void update() override;
    bool isPressed() const override;
    ButtonId getId() const override;
    void resetState() override;

private:
    ButtonConfig cfg_;
    std::unique_ptr<IPinReader> pinReader_;

    // État du bouton
    bool pressed_;           // État logique du bouton
    bool toggleState_;       // État pour mode toggle
    bool lastRawState_;      // Dernier état brut lu
    unsigned long lastDebounceTime_;  // Temps du dernier changement
    bool stableState_;       // État stable après debounce

    /**
     * @brief Lit l'état brut de la pin et applique la logique active-low
     */
    bool readCurrentState();
};