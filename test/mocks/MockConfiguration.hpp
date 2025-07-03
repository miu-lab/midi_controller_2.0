#pragma once

#include "core/domain/interfaces/IConfiguration.hpp"
#include "test/fixtures/test_configurations.hpp"
#include <vector>
#include <optional>
#include <map>

/**
 * @brief Mock pour IConfiguration permettant d'isoler les tests configuration
 * 
 * Utilise des données de test prédéfinies sans dépendre de la vraie configuration
 * du système.
 */
class MockConfiguration : public IConfiguration {
public:
    enum ConfigurationType {
        MINIMAL,
        COMPLEX, 
        ENCODER_WITH_CHILD_BUTTONS,
        EMPTY,
        INVALID
    };

    MockConfiguration(ConfigurationType type = MINIMAL) 
        : initialized_(false), debug_enabled_(true), midi_channel_(1), hardware_init_enabled_(true) {
        setConfigurationType(type);
    }

    // Interface IConfiguration
    Result<bool> init() override {
        if (should_fail_init_) {
            return Result<bool>::error({ErrorCode::InitializationFailed, "Mock init failure"});
        }
        initialized_ = true;
        return Result<bool>::success(true);
    }

    const std::vector<ControlDefinition>& getAllControlDefinitions() const override {
        return current_controls_;
    }

    std::vector<ControlDefinition> getControlDefinitionsByType(InputType type) const override {
        std::vector<ControlDefinition> filtered;
        for (const auto& control : current_controls_) {
            if (control.hardware.type == type) {
                filtered.push_back(control);
            }
        }
        return filtered;
    }

    std::optional<ControlDefinition> getControlDefinitionById(InputId id) const override {
        for (const auto& control : current_controls_) {
            if (control.id == id) {
                return control;
            }
        }
        return std::nullopt;
    }

    std::vector<ControlDefinition> getControlDefinitionsByGroup(const std::string& group) const override {
        std::vector<ControlDefinition> filtered;
        for (const auto& control : current_controls_) {
            if (control.group == group) {
                filtered.push_back(control);
            }
        }
        return filtered;
    }

    bool isNavigationControl(InputId id) const override {
        return navigation_controls_.find(id) != navigation_controls_.end();
    }

    void setControlForNavigation(InputId id, bool isNavigation) override {
        if (isNavigation) {
            navigation_controls_.insert(id);
        } else {
            navigation_controls_.erase(id);
        }
    }

    bool isDebugEnabled() const override {
        return debug_enabled_;
    }

    int midiChannel() const override {
        return midi_channel_;
    }

    bool isHardwareInitEnabled() const override {
        return hardware_init_enabled_;
    }

    bool validateAllConfigurations() const override {
        // Pour les tests, on peut simuler la validation
        return current_config_type_ != INVALID;
    }

    std::vector<std::string> getAvailableGroups() const override {
        std::set<std::string> unique_groups;
        for (const auto& control : current_controls_) {
            unique_groups.insert(control.group);
        }
        return std::vector<std::string>(unique_groups.begin(), unique_groups.end());
    }

    size_t getInputCountByType(InputType type) const override {
        size_t count = 0;
        for (const auto& control : current_controls_) {
            if (control.hardware.type == type) {
                count++;
            }
        }
        return count;
    }

    // Méthodes de test spécifiques
    void setConfigurationType(ConfigurationType type) {
        current_config_type_ = type;
        switch (type) {
            case MINIMAL:
                current_controls_ = TestConfigurations::getMinimalConfiguration();
                break;
            case COMPLEX:
                current_controls_ = TestConfigurations::getComplexConfiguration();
                break;
            case ENCODER_WITH_CHILD_BUTTONS:
                current_controls_ = TestConfigurations::getEncoderWithChildButtonsConfiguration();
                break;
            case EMPTY:
                current_controls_ = TestConfigurations::getEmptyConfiguration();
                break;
            case INVALID:
                current_controls_ = TestConfigurations::getInvalidConfiguration();
                break;
        }
    }

    void addControlDefinition(const ControlDefinition& control) {
        current_controls_.push_back(control);
    }

    void clearControlDefinitions() {
        current_controls_.clear();
    }

    void setShouldFailInit(bool fail) {
        should_fail_init_ = fail;
    }

    void setDebugEnabled(bool enabled) {
        debug_enabled_ = enabled;
    }

    void setMidiChannel(int channel) {
        midi_channel_ = channel;
    }

    void setHardwareInitEnabled(bool enabled) {
        hardware_init_enabled_ = enabled;
    }

    bool isInitialized() const {
        return initialized_;
    }

    void reset() {
        current_controls_.clear();
        navigation_controls_.clear();
        initialized_ = false;
        should_fail_init_ = false;
        debug_enabled_ = true;
        midi_channel_ = 1;
        hardware_init_enabled_ = true;
        setConfigurationType(MINIMAL);
    }

private:
    ConfigurationType current_config_type_;
    std::vector<ControlDefinition> current_controls_;
    std::set<InputId> navigation_controls_;
    bool initialized_;
    bool should_fail_init_ = false;
    bool debug_enabled_;
    int midi_channel_;
    bool hardware_init_enabled_;
};