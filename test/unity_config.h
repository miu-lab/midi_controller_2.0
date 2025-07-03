#pragma once

// Configuration Unity pour Teensy 4.1
// Configuration simplifiée sans inclusions problématiques

// Configuration mémoire pour Teensy
#define UNITY_EXCLUDE_FLOAT     // Exclure les tests float pour économiser la mémoire
#define UNITY_EXCLUDE_DOUBLE    // Exclure les tests double

// Configuration pour environnement embarqué
#define UNITY_EXCLUDE_DETAILS   // Réduire les détails pour économiser la mémoire
#define UNITY_EXCLUDE_SETJMP    // Pas de setjmp/longjmp sur embarqué

// Timeout pour les tests sur hardware
#define UNITY_TEST_TIMEOUT_MS   5000