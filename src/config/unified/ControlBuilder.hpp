#pragma once

#include "config/unified/ControlDefinition.hpp"
#include "core/domain/navigation/NavigationAction.hpp"

/**
 * @brief Builder fluide pour créer des définitions de contrôles
 * 
 * Simplifie la création de contrôles avec une API chainable et
 * gère automatiquement les conventions (ex: ID boutons encodeurs).
 */
class ControlBuilder {
public:
    ControlBuilder(InputId id, const std::string& name) {
        control_.id = id;
        control_.name = name;
        control_.label = name;  // Par défaut, peut être changé
    }
    
    // Constructeur alternatif pour const char*
    ControlBuilder(InputId id, const char* name) : ControlBuilder(id, std::string(name)) {}


    // === CONFIGURATION DE BASE ===

    ControlBuilder& withLabel(const std::string& label) {
        control_.label = label;
        return *this;
    }

    ControlBuilder& inGroup(const std::string& group) {
        control_.group = group;
        return *this;
    }

    ControlBuilder& withDescription(const std::string& desc) {
        control_.description = desc;
        return *this;
    }

    ControlBuilder& withDisplayOrder(uint8_t order) {
        control_.displayOrder = order;
        return *this;
    }

    ControlBuilder& disabled() {
        control_.enabled = false;
        return *this;
    }

    // === HARDWARE - ENCODEUR ===

    ControlBuilder& asRotaryEncoder(uint8_t pinA, uint8_t pinB, uint16_t ppr = 24) {
        control_.hardware.type = InputType::ENCODER;

        control_.hardware.config = ControlDefinition::EncoderConfig();
        auto& enc = std::get<ControlDefinition::EncoderConfig>(control_.hardware.config);
        enc.pinA = GpioPin{pinA, PinMode::PULLUP};
        enc.pinB = GpioPin{pinB, PinMode::PULLUP};
        enc.ppr = ppr;
        enc.sensitivity = 1.0f;
        enc.enableAcceleration = false;  // Désactivé par défaut
        enc.stepsPerDetent = 4;

        return *this;
    }


    ControlBuilder& withSensitivity(float sensitivity) {
        if (std::holds_alternative<ControlDefinition::EncoderConfig>(control_.hardware.config)) {
            auto& enc = std::get<ControlDefinition::EncoderConfig>(control_.hardware.config);
            enc.sensitivity = sensitivity;
        }
        return *this;
    }

    ControlBuilder& withStepPerDetent(bool enable = true, uint8_t stepsPerDetent = 4) {
        if (std::holds_alternative<ControlDefinition::EncoderConfig>(control_.hardware.config)) {
            auto& enc = std::get<ControlDefinition::EncoderConfig>(control_.hardware.config);
            enc.enableAcceleration = enable;
            enc.stepsPerDetent = stepsPerDetent;
        }
        return *this;
    }

    // === HARDWARE - BOUTON ===

    ControlBuilder& asButton(uint8_t pin, uint16_t debounceMs = 30, ButtonMode mode = ButtonMode::MOMENTARY) {
        control_.hardware.type = InputType::BUTTON;

        control_.hardware.config = ControlDefinition::ButtonConfig();
        auto& btn = std::get<ControlDefinition::ButtonConfig>(control_.hardware.config);
        btn.pin = GpioPin{pin, PinMode::PULLUP};
        btn.activeLow = true;
        btn.mode = mode;
        btn.debounceMs = debounceMs;

        return *this;
    }

    ControlBuilder& withLongPress(uint16_t ms = 1000) {
        if (std::holds_alternative<ControlDefinition::ButtonConfig>(control_.hardware.config)) {
            auto& btn = std::get<ControlDefinition::ButtonConfig>(control_.hardware.config);
            btn.longPressMs = ms;
        }
        return *this;
    }

    // === HIÉRARCHIE ===

    ControlBuilder& asChildOf(uint16_t parentId) {
        control_.parentId = parentId;
        return *this;
    }

    // === MAPPINGS ===

    ControlBuilder& withMidiCC(uint8_t cc, uint8_t channel = 0, bool relative = true) {
        ControlDefinition::MappingSpec mapping;
        mapping.role = MappingRole::MIDI;
        mapping.appliesTo = MappingControlType::ENCODER;

        mapping.config = ControlDefinition::MidiConfig();
        auto& midi = std::get<ControlDefinition::MidiConfig>(mapping.config);
        midi.channel = channel;
        midi.control = cc;
        midi.isRelative = relative;

        control_.mappings.push_back(mapping);
        return *this;
    }

    ControlBuilder& withMidiNote(uint8_t note, uint8_t channel = 0) {
        ControlDefinition::MappingSpec mapping;
        mapping.role = MappingRole::MIDI;
        mapping.appliesTo = MappingControlType::BUTTON;

        mapping.config = ControlDefinition::MidiConfig();
        auto& midi = std::get<ControlDefinition::MidiConfig>(mapping.config);
        midi.channel = channel;
        midi.control = note;
        midi.isRelative = false;

        control_.mappings.push_back(mapping);
        return *this;
    }

    ControlBuilder& withNavigation(NavigationAction action, MappingControlType appliesTo = MappingControlType::ENCODER, int parameter = 0) {
        ControlDefinition::MappingSpec mapping;
        mapping.role = MappingRole::NAVIGATION;
        mapping.appliesTo = appliesTo;

        mapping.config = ControlDefinition::NavigationConfig();
        auto& nav = std::get<ControlDefinition::NavigationConfig>(mapping.config);
        nav.action = action;
        nav.parameter = parameter;

        control_.mappings.push_back(mapping);
        return *this;
    }

    // === MÉTHODES HELPER POUR ACTIONS COURANTES ===

    ControlBuilder& asHomeButton() {
        return withNavigation(NavigationAction::HOME, MappingControlType::BUTTON);
    }

    ControlBuilder& asBackButton() {
        return withNavigation(NavigationAction::BACK, MappingControlType::BUTTON);
    }

    ControlBuilder& asItemNavigator() {
        return withNavigation(NavigationAction::ITEM_NAVIGATOR, MappingControlType::ENCODER);
    }

    ControlBuilder& asItemValidator() {
        return withNavigation(NavigationAction::ITEM_VALIDATE, MappingControlType::BUTTON);
    }

    ControlBuilder& asMenuEnterButton() {
        return withNavigation(NavigationAction::MENU_ENTER, MappingControlType::BUTTON);
    }

    ControlBuilder& asMenuExitButton() {
        return withNavigation(NavigationAction::MENU_EXIT, MappingControlType::BUTTON);
    }

    ControlBuilder& asParameterEditor() {
        return withNavigation(NavigationAction::PARAMETER_EDIT, MappingControlType::ENCODER);
    }

    ControlBuilder& asParameterValidator() {
        return withNavigation(NavigationAction::PARAMETER_VALIDATE, MappingControlType::BUTTON);
    }


    // === BUILD ===

    ControlDefinition build() {
        // Validation basique
        if (control_.id == 0) {
            // Control ID cannot be 0 - this should be checked before calling build()
            return control_; // Return invalid control
        }

        // Auto-génération du label si nécessaire
        if (control_.label.empty()) {
            control_.label = control_.name;
        }

        return control_;
    }

private:
    ControlDefinition control_;
};
