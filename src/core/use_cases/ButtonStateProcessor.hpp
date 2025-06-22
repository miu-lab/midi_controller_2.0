#pragma once

#include <functional>
#include <vector>

/**
 * @brief Template pour traiter les changements d'état de boutons
 * 
 * Factorisation de la logique commune entre boutons normaux et boutons d'encodeurs.
 * Évite la duplication de code tout en gardant les performances optimales.
 */
template<typename Port>
void processButtonChanges(const std::vector<Port*>& ports, 
                         std::vector<bool>& lastStates,
                         std::function<void(uint8_t, bool)> callback) {
    for (size_t i = 0; i < ports.size(); ++i) {
        bool pressed = ports[i]->isPressed();
        if (pressed != lastStates[i]) {
            lastStates[i] = pressed;
            callback(ports[i]->getId(), pressed);
        }
    }
}