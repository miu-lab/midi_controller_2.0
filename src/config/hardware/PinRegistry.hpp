#pragma once

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include "config/ETLConfig.hpp"

/**
 * @brief Registre centralisé de toutes les affectations de pins
 *
 * Permet de détecter les conflits d'affectation et de générer
 * un rapport détaillé de l'utilisation des pins.
 */
class PinRegistry {
public:
    enum class PinUsage {
        ENCODER_A,
        ENCODER_B,
        BUTTON,
        DISPLAY_CS,
        DISPLAY_DC,
        DISPLAY_RST,
        DISPLAY_MOSI,
        DISPLAY_MISO,
        DISPLAY_SCK,
        MUX_S0,
        MUX_S1,
        MUX_S2,
        MUX_S3,
        MUX_ENABLE,
        MUX_SIGNAL,
        I2C_SDA,
        I2C_SCL,
        UART_TX,
        UART_RX,
        UNUSED
    };

    struct PinAssignment {
        uint8_t pin;
        PinUsage usage;
        std::string component;
        std::string description;
    };

    /**
     * @brief Enregistre une affectation de pin
     * @return true si l'enregistrement est réussi, false si conflit
     */
    bool registerPin(uint8_t pin, PinUsage usage,
                     const std::string& component,
                     const std::string& description = "") {

        // Vérifier si la pin est déjà utilisée
        auto it = assignments_.find(pin);
        if (it != assignments_.end()) {
            // Conflit détecté
            conflicts_.push_back({
                pin,
                it->second.component + " (" + usageToString(it->second.usage) + ")",
                component + " (" + usageToString(usage) + ")"
            });
            return false;
        }

        // Enregistrer l'affectation
        assignments_[pin] = {pin, usage, component, description};
        return true;
    }

    /**
     * @brief Enregistre plusieurs pins en une fois
     */
    void registerMultiple(const std::vector<PinAssignment>& assignments) {
        for (const auto& assign : assignments) {
            registerPin(assign.pin, assign.usage, assign.component, assign.description);
        }
    }

    /**
     * @brief Vérifie s'il y a des conflits
     */
    bool hasConflicts() const {
        return !conflicts_.empty();
    }

    /**
     * @brief Génère un rapport complet de l'utilisation des pins
     */
    std::string generateReport() const {
        std::stringstream report;

        report << "\n========================================\n";
        report << "     PIN ALLOCATION REPORT\n";
        report << "========================================\n\n";

        // Statistiques
        report << "Total pins used: " << assignments_.size() << " / 55 (Teensy 4.1)\n\n";

        // Conflits
        if (hasConflicts()) {
            report << "⚠️  CONFLICTS DETECTED:\n";
            report << "------------------------\n";
            for (const auto& conflict : conflicts_) {
                report << "Pin " << static_cast<int>(conflict.pin)
                       << ": " << conflict.firstUser
                       << " vs " << conflict.secondUser << "\n";
            }
            report << "\n";
        } else {
            report << "✅ No conflicts detected\n\n";
        }

        // Affectations par catégorie
        report << "PIN ASSIGNMENTS BY CATEGORY:\n";
        report << "-----------------------------\n\n";

        // Grouper par type d'usage
        std::map<PinUsage, std::vector<PinAssignment>> byUsage;
        for (const auto& [pin, assign] : assignments_) {
            byUsage[assign.usage].push_back(assign);
        }

        for (const auto& [usage, assigns] : byUsage) {
            report << usageToString(usage) << ":\n";
            for (const auto& assign : assigns) {
                report << "  Pin " << std::setw(2) << static_cast<int>(assign.pin)
                       << " - " << assign.component;
                if (!assign.description.empty()) {
                    report << " (" << assign.description << ")";
                }
                report << "\n";
            }
            report << "\n";
        }

        // Pins disponibles
        report << "AVAILABLE PINS:\n";
        report << "---------------\n";
        std::vector<uint8_t> available;
        for (uint8_t pin = 0; pin < 55; ++pin) {
            if (assignments_.find(pin) == assignments_.end()) {
                available.push_back(pin);
            }
        }

        if (available.empty()) {
            report << "No pins available!\n";
        } else {
            for (size_t i = 0; i < available.size(); ++i) {
                report << std::setw(2) << static_cast<int>(available[i]);
                if ((i + 1) % 10 == 0) report << "\n";
                else report << ", ";
            }
            if (available.size() % 10 != 0) report << "\n";
        }

        report << "\n========================================\n";

        return report.str();
    }

    /**
     * @brief Réinitialise le registre
     */
    void clear() {
        assignments_.clear();
        conflicts_.clear();
    }

    /**
     * @brief Instance singleton
     */
    static PinRegistry& getInstance() {
        static PinRegistry instance;
        return instance;
    }

private:
    PinRegistry() = default;

    struct Conflict {
        uint8_t pin;
        std::string firstUser;
        std::string secondUser;
    };

    std::map<uint8_t, PinAssignment> assignments_;
    std::vector<Conflict> conflicts_;

    static std::string usageToString(PinUsage usage) {
        switch (usage) {
            case PinUsage::ENCODER_A: return "Encoder A";
            case PinUsage::ENCODER_B: return "Encoder B";
            case PinUsage::BUTTON: return "Button";
            case PinUsage::DISPLAY_CS: return "Display CS";
            case PinUsage::DISPLAY_DC: return "Display DC";
            case PinUsage::DISPLAY_RST: return "Display RST";
            case PinUsage::DISPLAY_MOSI: return "Display MOSI";
            case PinUsage::DISPLAY_MISO: return "Display MISO";
            case PinUsage::DISPLAY_SCK: return "Display SCK";
            case PinUsage::MUX_S0: return "Mux S0";
            case PinUsage::MUX_S1: return "Mux S1";
            case PinUsage::MUX_S2: return "Mux S2";
            case PinUsage::MUX_S3: return "Mux S3";
            case PinUsage::MUX_ENABLE: return "Mux Enable";
            case PinUsage::MUX_SIGNAL: return "Mux Signal";
            case PinUsage::I2C_SDA: return "I2C SDA";
            case PinUsage::I2C_SCL: return "I2C SCL";
            case PinUsage::UART_TX: return "UART TX";
            case PinUsage::UART_RX: return "UART RX";
            case PinUsage::UNUSED: return "Unused";
            default: return "Unknown";
        }
    }
};