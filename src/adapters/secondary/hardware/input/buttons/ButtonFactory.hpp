#pragma once

#include <memory>
#include "adapters/secondary/hardware/input/buttons/ButtonConfig.hpp"
#include "adapters/secondary/hardware/input/buttons/UnifiedButton.hpp"
#include "adapters/secondary/hardware/input/buttons/readers/IPinReader.hpp"

/**
 * @brief Factory pour créer des boutons avec la bonne stratégie de lecture
 *
 * Cette factory analyse la configuration GPIO et créé automatiquement
 * le bon type de lecteur (direct ou multiplexé).
 */
class ButtonFactory {
public:
    /**
     * @brief Crée un bouton avec la stratégie appropriée
     * @param config Configuration du bouton
     * @return Bouton unifié prêt à l'emploi
     */
    static std::unique_ptr<UnifiedButton> createButton(const ButtonConfig& config);

private:
    /**
     * @brief Crée le bon lecteur selon la configuration GPIO
     * @param gpio Configuration de la pin
     * @return Lecteur de pin approprié
     */
    static std::unique_ptr<IPinReader> createPinReader(const GpioPin& gpio);
};