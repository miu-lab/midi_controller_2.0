#pragma once
#include <optional>
#include <variant>
#include <type_traits>

#include "core/domain/types.hpp"
#include "adapters/secondary/hardware/input/buttons/ButtonConfig.hpp"
#include "adapters/secondary/hardware/input/encoders/EncoderConfig.hpp"

/**
 * @brief Configuration générique pour un contrôle d'entrée (encodeur ou bouton).
 */
struct InputConfig {
    InputId id;                                      ///< Identifiant unique du contrôle
    InputName name;                                  ///< Nom technique du contrôle
    InputType type;                                  ///< Type de contrôle (ENCODER ou BUTTON)
    InputLabel label;                                ///< Étiquette d'affichage
    std::variant<ButtonConfig, EncoderConfig> config; ///< Configuration spécifique
    
    // === MÉTADONNÉES UI ===
    InputGroup group = "General";                    ///< Groupe logique pour l'UI
    InputDescription description = "";               ///< Description détaillée
    bool enabled = true;                             ///< Contrôle actif/inactif
    uint8_t displayOrder = 0;                        ///< Ordre d'affichage dans l'UI
    
    /**
     * @brief Récupère la configuration typée de manière sécurisée
     * @tparam T Type de configuration attendu (ButtonConfig ou EncoderConfig)
     * @return Configuration si le type correspond, std::nullopt sinon
     */
    template <typename T>
    std::optional<T> getConfig() const {
        // Vérification de cohérence type/configuration
        if constexpr (std::is_same_v<T, ButtonConfig>) {
            if (type != InputType::BUTTON) {
                return std::nullopt;  // Type incohérent
            }
        } else if constexpr (std::is_same_v<T, EncoderConfig>) {
            if (type != InputType::ENCODER) {
                return std::nullopt;  // Type incohérent
            }
        } else {
            static_assert(std::is_same_v<T, ButtonConfig> || std::is_same_v<T, EncoderConfig>, 
                         "T doit être ButtonConfig ou EncoderConfig");
        }
        
        // Extraction sécurisée du variant
        if (auto* cfg = std::get_if<T>(&config)) {
            return *cfg;
        }
        return std::nullopt;
    }
    
    /**
     * @brief Vérifie si la configuration globale est valide
     * @return true si tous les paramètres sont cohérents
     */
    bool isValid() const {
        // Vérification des champs obligatoires
        if (id == 0 || name.empty() || label.empty()) {
            return false;
        }
        
        // Validation de la configuration spécifique
        bool configValid = std::visit([](const auto& cfg) { 
            return cfg.isValid(); 
        }, config);
        
        if (!configValid) {
            return false;
        }
        
        // Vérification de cohérence type/variant
        bool typeMatch = (type == InputType::BUTTON && std::holds_alternative<ButtonConfig>(config)) ||
                        (type == InputType::ENCODER && std::holds_alternative<EncoderConfig>(config));
        
        return typeMatch;
    }
    
    /**
     * @brief Obtient l'ID physique principal (GPIO ou ID encodeur)
     * @return ID principal du composant physique
     */
    uint16_t getPrimaryPhysicalId() const {
        return std::visit([](const auto& cfg) -> uint16_t {
            if constexpr (std::is_same_v<std::decay_t<decltype(cfg)>, ButtonConfig>) {
                return cfg.gpio.pin;
            } else {
                return cfg.id;
            }
        }, config);
    }
    
    /**
     * @brief Vérifie si ce contrôle a un bouton
     * @return true si c'est un bouton ou un encodeur avec bouton
     */
    bool hasButton() const {
        if (type == InputType::BUTTON) {
            return true;
        } else if (type == InputType::ENCODER) {
            if (auto encConfig = getConfig<EncoderConfig>()) {
                return encConfig->buttonConfig.has_value();
            }
        }
        return false;
    }
    
    /**
     * @brief Obtient la configuration du bouton (si présent)
     * @return Configuration du bouton ou std::nullopt
     */
    std::optional<ButtonConfig> getButtonConfig() const {
        if (type == InputType::BUTTON) {
            return getConfig<ButtonConfig>();
        } else if (type == InputType::ENCODER) {
            if (auto encConfig = getConfig<EncoderConfig>()) {
                return encConfig->buttonConfig;
            }
        }
        return std::nullopt;
    }
};
