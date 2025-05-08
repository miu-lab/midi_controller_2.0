#pragma once

#include "config/common/CommonIncludes.hpp"

/**
 * @brief Fichier d'inclusion pour le système d'événements
 */

// Classe de base des événements
#include "core/domain/events/core/Event.hpp"

// Types d'événements spécifiques
#include "core/domain/events/core/EventTypes.hpp"

// Bus d'événements
#include "core/domain/events/core/EventBus.hpp"

// Dispatcheur d'événements
#include "core/domain/events/core/EventDispatcher.hpp"
