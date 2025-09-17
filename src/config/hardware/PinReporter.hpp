#pragma once

#include "config/hardware/PinValidator.hpp"
#include "config/unified/ConfigurationFactory.hpp"
#include <Arduino.h>

/**
 * @brief Générateur de rapport d'utilisation des pins à la compilation
 *
 * Utilise le PinValidator pour générer automatiquement un rapport
 * lors du démarrage du système ou à la demande.
 */
class PinReporter {
public:
    /**
     * @brief Génère et affiche le rapport complet d'utilisation des pins
     *
     * Cette fonction doit être appelée au démarrage pour vérifier
     * la configuration et afficher le rapport en mode debug.
     */
    static void generateStartupReport() {
        Serial.println(F("\n========================================"));
        Serial.println(F("   MIDI STUDIO PIN CONFIGURATION"));
        Serial.println(F("========================================"));

        // Créer la configuration par défaut
        auto config = ConfigurationFactory::createDefaultConfiguration();

        // Valider et générer le rapport
        bool isValid = PinValidator::validateAndReport(*config);

        if (!isValid) {
            Serial.println(F("\n⚠️  CRITICAL ERROR: Pin conflicts detected!"));
            Serial.println(F("⚠️  Please check the pin assignments above."));
            Serial.println(F("⚠️  System may not function correctly."));

            // En cas de conflit critique, on peut décider d'arrêter le système
#ifdef HALT_ON_PIN_CONFLICT
            Serial.println(F("\n🛑 SYSTEM HALTED due to pin conflicts"));
            while (1) {
                // Faire clignoter la LED pour indiquer l'erreur
                digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
                delay(500);
            }
#endif
        } else {
            Serial.println(F("\n✅ Pin configuration validated successfully"));
        }

        Serial.println(F("========================================\n"));
    }

    /**
     * @brief Vérifie une pin spécifique et retourne son utilisation
     */
    static std::string checkPin(uint8_t pin) {
        auto& registry = PinRegistry::getInstance();

        // Réinitialiser et reconstruire le registre
        registry.clear();
        auto config = ConfigurationFactory::createDefaultConfiguration();
        PinValidator::validateAndReport(*config);

        // Générer un rapport simple pour cette pin
        return registry.generateReport(); // Pour l'instant retourne tout
    }

    /**
     * @brief Active/désactive l'affichage automatique du rapport au démarrage
     */
    static void setAutoReportEnabled(bool enabled) {
        autoReportEnabled_ = enabled;
    }

    /**
     * @brief Vérifie si le rapport automatique est activé
     */
    static bool isAutoReportEnabled() {
        return autoReportEnabled_;
    }

private:
    static bool autoReportEnabled_;
};

// Initialisation de la variable statique
bool PinReporter::autoReportEnabled_ = true; // Activé par défaut en debug

/**
 * @brief Macro pour faciliter l'appel du rapport au démarrage
 *
 * À utiliser dans setup() de votre main.cpp :
 * ```cpp
 * void setup() {
 *     Serial.begin(115200);
 *     REPORT_PIN_CONFIGURATION();
 *     // ... reste de l'initialisation
 * }
 * ```
 */
#ifdef DEBUG_BUILD
    #define REPORT_PIN_CONFIGURATION() PinReporter::generateStartupReport()
#else
    // En release, ne génère le rapport que s'il y a des conflits
    #define REPORT_PIN_CONFIGURATION() \
        do { \
            auto config = ConfigurationFactory::createDefaultConfiguration(); \
            if (!PinValidator::validateAndReport(*config)) { \
                PinReporter::generateStartupReport(); \
            } \
        } while(0)
#endif

/**
 * @brief Macro pour validation stricte à la compilation
 *
 * Utiliser dans un fichier source pour forcer la vérification :
 * ```cpp
 * ENFORCE_PIN_VALIDATION();
 * ```
 */
#define ENFORCE_PIN_VALIDATION() VALIDATE_PIN_CONFIGURATION()