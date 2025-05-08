#pragma once
#include "core/domain/types.hpp"
#include <vector>
#include <cstddef>  // Pour size_t

/**
 * @brief Structure définissant un contrôle (encodeur ou bouton) dédié à la navigation
 */
struct NavigationControl {
    ControlId id;
};

// Configuration des contrôles dédiés à la navigation
extern const std::vector<NavigationControl> navigationControls;
extern const std::size_t navigationControlCount;
