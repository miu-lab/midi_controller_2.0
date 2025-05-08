#pragma once

/**
 * @brief Inclusions communes utilisées dans l'ensemble du projet
 * Centralisé pour éviter les problèmes d'inclusions manquantes et assurer la cohérence
 */

// Inclusions standard
#include <Arduino.h>     // Base Arduino pour les microcontrôleurs
#include <stdint.h>      // Types entiers de taille fixe (uint8_t, etc.)
#include <cstddef>       // Pour size_t et nullptr
#include <stddef.h>      // Alternative à cstddef pour C

// Définitions globales utiles
#ifndef NULL
#define NULL 0
#endif

// Macro pour éviter les warnings de variables non utilisées
#define UNUSED(x) (void)(x)
