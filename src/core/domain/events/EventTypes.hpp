#pragma once

/**
 * @brief Types d'événements pour le système
 * Fichier de compatibilité pour supporter les fichiers existants qui l'incluent
 */

#include "core/domain/events/core/EventTypes.hpp"

// Structures compatibles avec l'ancienne API pour maintenir la compatibilité
namespace EventTypes {
    // Structure pour les événements d'encodeur tourné
    struct EncoderTurned {
        uint8_t id;                 // Identifiant de l'encodeur
        int32_t absolutePosition;   // Position absolue
    };

    // Structure pour les événements de bouton d'encodeur
    struct EncoderButton {
        uint8_t id;     // Identifiant de l'encodeur
        bool pressed;   // État du bouton
    };

    // Structure pour les événements de bouton pressé
    struct ButtonPressed {
        uint8_t id;   // Identifiant du bouton
    };

    // Structure pour les événements de bouton relâché
    struct ButtonReleased {
        uint8_t id;   // Identifiant du bouton
    };
}
