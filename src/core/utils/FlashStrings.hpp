#pragma once

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <WString.h>

/**
 * @brief Utilitaire pour la gestion des chaînes de caractères en mémoire Flash
 * 
 * Cette classe fournit des méthodes et macros pour stocker et utiliser des chaînes
 * de caractères en mémoire Flash (PROGMEM) plutôt qu'en RAM, permettant
 * d'économiser significativement la mémoire RAM.
 */
class FlashStrings {
public:
    /**
     * @brief Copie une chaîne de la mémoire Flash vers un buffer en RAM
     * @param buffer Buffer de destination en RAM
     * @param bufferSize Taille du buffer en octets
     * @param flashStr Chaîne stockée en mémoire Flash (PROGMEM)
     * @return Pointeur vers le buffer
     */
    static char* copy(char* buffer, size_t bufferSize, const char* flashStr) {
        if (!buffer || bufferSize <= 1 || !flashStr) return buffer;
        
        // Obtenir la longueur de la chaîne source
        size_t srcLen = strlen_P(flashStr);
        
        // Vérifier si la chaîne va être tronquée
        if (srcLen >= bufferSize) {
            #ifdef DEBUG
            // En mode debug, afficher un avertissement la première fois
            static bool warningDisplayed = false;
            if (!warningDisplayed) {
                Serial.println(F("AVERTISSEMENT: Troncation de chaîne détectée dans FlashStrings::copy"));
                warningDisplayed = true; // N'afficher qu'une fois
            }
            #endif
        }
        
        // Méthode de copie plus sûre pour éviter les avertissements de troncation
        if (srcLen >= bufferSize) {
            // Cas où la source est plus grande que la destination - copier avec troncation
            memcpy_P(buffer, flashStr, bufferSize - 1);
            buffer[bufferSize - 1] = '\0';
        } else {
            // Cas où la source tient dans la destination - copier le tout
            memcpy_P(buffer, flashStr, srcLen);
            buffer[srcLen] = '\0';
        }
        
        return buffer;
    }
    
    /**
     * @brief Obtient la longueur d'une chaîne stockée en mémoire Flash
     * @param flashStr Chaîne stockée en mémoire Flash (PROGMEM)
     * @return Longueur de la chaîne
     */
    static size_t length(const char* flashStr) {
        if (!flashStr) return 0;
        return strlen_P(flashStr);
    }
    
    /**
     * @brief Compare une chaîne en RAM avec une chaîne en mémoire Flash
     * @param str1 Chaîne en RAM
     * @param flashStr Chaîne en mémoire Flash (PROGMEM)
     * @return 0 si égales, < 0 si str1 < flashStr, > 0 si str1 > flashStr
     */
    static int compare(const char* str1, const char* flashStr) {
        if (!str1 || !flashStr) return 0;
        return strcmp_P(str1, flashStr);
    }
    
    /**
     * @brief Convertit une chaîne Flash en String Arduino
     * @param flashStr Chaîne stockée en mémoire Flash (PROGMEM)
     * @return String Arduino avec le contenu de la chaîne Flash
     */
    static String toString(const char* flashStr) {
        if (!flashStr) return String();
        
        size_t len = length(flashStr);
        String result;
        result.reserve(len);
        
        for (size_t i = 0; i < len; i++) {
            result += (char)pgm_read_byte(flashStr + i);
        }
        
        return result;
    }
};

/**
 * @brief Macro pour déclarer une chaîne en mémoire Flash
 * 
 * Exemple d'utilisation:
 * FLASH_STRING(ERROR_MESSAGE, "Erreur de communication");
 */
#define FLASH_STRING(name, value) const char name[] PROGMEM = value

/**
 * @brief Macro pour déclarer un tableau de chaînes en mémoire Flash
 * 
 * Exemple d'utilisation:
 * FLASH_STRING_ARRAY(MENU_ITEMS, "Item 1", "Item 2", "Item 3");
 */
#define FLASH_STRING_ARRAY(name, ...) const char* const name[] PROGMEM = { __VA_ARGS__ }

/**
 * @brief Macro pour obtenir un élément d'un tableau de chaînes en mémoire Flash
 * 
 * Exemple d'utilisation:
 * const char* item = FLASH_STRING_ARRAY_GET(MENU_ITEMS, 0);
 */
#define FLASH_STRING_ARRAY_GET(arr, idx) (const char*)pgm_read_word(&(arr)[idx])
