#pragma once

#include <Arduino.h>

#include <string>

#include "core/utils/Result.hpp"

namespace ErrorUtils {
/**
 * @brief Affiche un message d'erreur sur le port série
 *
 * @param result Le résultat contenant l'erreur à afficher
 * @param context Message de contexte indiquant où l'erreur s'est produite
 */
template <typename T>
void printError(const Result<T, std::string>& result, const char* context) {
    Serial.print(F("ERREUR: "));
    Serial.print(context);
    Serial.print(F(" - "));

    if (result.isError() && result.error()) {
        Serial.println(result.error()->c_str());
    } else {
        Serial.println(F("Erreur inconnue"));
    }
}
}  // namespace ErrorUtils
