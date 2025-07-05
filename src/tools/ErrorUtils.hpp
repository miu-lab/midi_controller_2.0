#pragma once

#include <Arduino.h>

#include "core/utils/Result.hpp"
#include "core/utils/Error.hpp"

namespace ErrorUtils {
/**
 * @brief Affiche un message d'erreur sur le port série
 *
 * @param result Le résultat contenant l'erreur à afficher
 * @param context Message de contexte indiquant où l'erreur s'est produite
 */
template <typename T>
void printError(const Result<T>& result, const char* context) {
    Serial.print(F("ERREUR: "));
    Serial.print(context);
    Serial.print(F(" - "));

    if (result.isError() && result.error()) {
        const Error& error = result.error().value();
        Serial.print(F("Code: "));
        Serial.print(static_cast<uint8_t>(error.code));
        Serial.print(F(", Message: "));
        Serial.println(error.message);
    } else {
        Serial.println(F("Erreur inconnue"));
    }
}

/**
 * @brief Affiche un message d'erreur pour une structure Error
 *
 * @param error L'erreur à afficher
 * @param context Message de contexte indiquant où l'erreur s'est produite
 */
void printError(const Error& error, const char* context) {
    Serial.print(F("ERREUR: "));
    Serial.print(context);
    Serial.print(F(" - Code: "));
    Serial.print(static_cast<uint8_t>(error.code));
    Serial.print(F(", Message: "));
    Serial.println(error.message);
}
}  // namespace ErrorUtils
