#include <Arduino.h>

#include "ConfigurationMidiExtractor.hpp"

ConfigurationMidiExtractor::ConfigurationMidiExtractor(const ParserConfig& config)
    : config_(config) {}

std::vector<ConfigurationMidiExtractor::MidiControlInfo>
ConfigurationMidiExtractor::extractMidiControls(const UnifiedConfiguration& configuration) const {
    std::vector<MidiControlInfo> midiControls;
    
    logInfo("Starting MIDI control extraction");
    
    // Obtenir tous les contrôles depuis la configuration
    auto allControls = configuration.getAllControls();
    logInfo("Found " + String(allControls.size()) + " total controls in configuration");
    
    for (const auto& control : allControls) {
        // Chercher les mappings MIDI pour ce contrôle
        auto midiMappings = control.getMappingsForRole(MappingRole::MIDI);
        
        if (!midiMappings.empty()) {
            logInfo("Control " + String(control.id) + " (" + String(control.label.c_str()) + ") has " + String(midiMappings.size()) + " MIDI mappings");
        }
        
        for (const auto& mapping : midiMappings) {
            if (std::holds_alternative<ControlDefinition::MidiConfig>(mapping.config)) {
                // Extraire seulement les contrôles MIDI des encodeurs (pas les boutons)
                auto midiInfo = extractFromEncoder(control);
                if (midiInfo.has_value()) {
                    auto validationResult = validateMidiControlInfo(midiInfo.value());
                    if (validationResult.isSuccess()) {
                        midiControls.push_back(midiInfo.value());
                        logInfo("Added ENCODER MIDI control: ID=" + String(midiInfo->control_id) + 
                               ", CC=" + String(midiInfo->cc_number) + 
                               ", CH=" + String(midiInfo->channel) + 
                               ", Name=" + midiInfo->name);
                    } else {
                        logError("Invalid MIDI control extracted for ID=" + String(control.id));
                    }
                } else {
                    logInfo("Skipped non-encoder MIDI control: ID=" + String(control.id) + 
                           ", Type=" + String(static_cast<int>(control.hardware.type)));
                }
            }
        }
    }
    
    logInfo("Extracted " + String(midiControls.size()) + " MIDI controls total");
    return midiControls;
}

std::vector<ConfigurationMidiExtractor::ButtonInfo> ConfigurationMidiExtractor::extractButtonInfo(
    const UnifiedConfiguration& configuration) const {
    std::vector<ButtonInfo> buttonInfos;
    
    logInfo("Starting button extraction");
    
    // Obtenir tous les contrôles depuis la configuration
    auto allControls = configuration.getAllControls();
    logInfo("Found " + String(allControls.size()) + " total controls in configuration");
    
    for (const auto& control : allControls) {
        // Extraire les boutons autonomes
        if (control.hardware.type == InputType::BUTTON && control.enabled) {
            auto buttonInfo = extractButtonFromControl(control);
            if (buttonInfo.has_value()) {
                auto validationResult = validateButtonInfo(buttonInfo.value());
                if (validationResult.isSuccess()) {
                    buttonInfos.push_back(buttonInfo.value());
                    if (buttonInfo->hasParent()) {
                        logInfo("Found button " + String(buttonInfo->button_id) + " with parent " + String(buttonInfo->parent_encoder_id));
                    } else {
                        logInfo("Found standalone button " + String(buttonInfo->button_id));
                    }
                }
            }
        }
        
        // Vérifier aussi les boutons intégrés aux encodeurs
        if (control.hardware.type == InputType::ENCODER && control.enabled) {
            if (control.hardware.encoderButtonPin.has_value()) {
                ButtonInfo info;
                info.button_id = control.getEncoderButtonId();  // ID encodeur + 1000
                info.parent_encoder_id = control.id;  // L'encodeur est le parent
                info.name = String(control.label.c_str()) + " BTN";
                
                auto validationResult = validateButtonInfo(info);
                if (validationResult.isSuccess()) {
                    logInfo("Found integrated encoder button " + String(info.button_id) + " for encoder " + String(info.parent_encoder_id));
                    buttonInfos.push_back(info);
                }
            }
        }
    }
    
    logInfo("Extracted " + String(buttonInfos.size()) + " button infos from config");
    return buttonInfos;
}

Result<void> ConfigurationMidiExtractor::validateMidiControlInfo(const MidiControlInfo& info) const {
    // Valider le numéro CC
    if (info.cc_number > config_.maxCCNumber) {
        logError("Invalid CC number: " + String(info.cc_number) + " > " + String(config_.maxCCNumber));
        return Result<void>::error({ErrorCode::ConfigurationError, "Invalid CC number"});
    }
    
    // Valider le canal MIDI (0-15 interne, 1-16 externe)
    if (info.channel > 15) {
        logError("Invalid MIDI channel: " + String(info.channel));
        return Result<void>::error({ErrorCode::ConfigurationError, "Invalid MIDI channel"});
    }
    
    // Valider que le nom n'est pas vide
    if (info.name.length() == 0) {
        logError("Empty control name for ID=" + String(info.control_id));
        return Result<void>::error({ErrorCode::ConfigurationError, "Empty control name"});
    }
    
    return Result<void>::success();
}

Result<void> ConfigurationMidiExtractor::validateButtonInfo(const ButtonInfo& info) const {
    // Valider l'ID du bouton
    if (info.button_id == 0) {
        logError("Invalid button ID: 0");
        return Result<void>::error({ErrorCode::ConfigurationError, "Invalid button ID"});
    }
    
    // Valider que le nom n'est pas vide
    if (info.name.length() == 0) {
        logError("Empty button name for ID=" + String(info.button_id));
        return Result<void>::error({ErrorCode::ConfigurationError, "Empty button name"});
    }
    
    return Result<void>::success();
}

std::optional<ConfigurationMidiExtractor::MidiControlInfo>
ConfigurationMidiExtractor::extractFromEncoder(const ControlDefinition& controlDef) const {
    // Vérifier que c'est bien un encodeur
    if (controlDef.hardware.type != InputType::ENCODER || !controlDef.enabled) {
        return std::nullopt;
    }
    
    // Chercher les mappings MIDI pour ce contrôle
    auto midiMappings = controlDef.getMappingsForRole(MappingRole::MIDI);
    
    for (const auto& mapping : midiMappings) {
        if (std::holds_alternative<ControlDefinition::MidiConfig>(mapping.config)) {
            auto midiConfig = std::get<ControlDefinition::MidiConfig>(mapping.config);
            
            MidiControlInfo info;
            info.cc_number = midiConfig.control;
            info.channel = midiConfig.channel;
            
            // Gérer la concaténation des strings correctement
            if (controlDef.label.length() > 0) {
                info.name = controlDef.label.c_str();
            } else {
                info.name = "CC" + String(midiConfig.control);
            }
            
            info.control_id = controlDef.id;
            return info;
        }
    }
    
    return std::nullopt;
}

std::optional<ConfigurationMidiExtractor::MidiControlInfo>
ConfigurationMidiExtractor::extractFromButton(const ControlDefinition& controlDef) const {
    // Vérifier que c'est bien un bouton
    if (controlDef.hardware.type != InputType::BUTTON || !controlDef.enabled) {
        return std::nullopt;
    }
    
    // Chercher les mappings MIDI pour ce contrôle
    auto midiMappings = controlDef.getMappingsForRole(MappingRole::MIDI);
    
    for (const auto& mapping : midiMappings) {
        if (std::holds_alternative<ControlDefinition::MidiConfig>(mapping.config)) {
            auto midiConfig = std::get<ControlDefinition::MidiConfig>(mapping.config);
            
            MidiControlInfo info;
            info.cc_number = midiConfig.control;
            info.channel = midiConfig.channel;
            
            // Gérer la concaténation des strings correctement
            if (controlDef.label.length() > 0) {
                info.name = controlDef.label.c_str();
            } else {
                info.name = "BTN" + String(midiConfig.control);
            }
            
            info.control_id = controlDef.id;
            return info;
        }
    }
    
    return std::nullopt;
}

std::optional<ConfigurationMidiExtractor::ButtonInfo>
ConfigurationMidiExtractor::extractButtonFromControl(const ControlDefinition& controlDef) const {
    // Vérifier que c'est bien un bouton
    if (controlDef.hardware.type != InputType::BUTTON || !controlDef.enabled) {
        return std::nullopt;
    }
    
    ButtonInfo info;
    info.button_id = controlDef.id;
    info.name = controlDef.label.length() > 0 ? String(controlDef.label.c_str()) : ("BTN" + String(controlDef.id));
    
    // Déterminer si ce bouton a un parent encodeur
    if (controlDef.parentId.has_value()) {
        info.parent_encoder_id = controlDef.parentId.value();
    } else {
        info.parent_encoder_id = 0;  // Pas de parent
    }
    
    return info;
}

void ConfigurationMidiExtractor::logInfo(const String& message) const {
    if (config_.enableLogging) {
        Serial.println("[ConfigurationMidiExtractor] " + message);
    }
}

void ConfigurationMidiExtractor::logError(const String& message) const {
    if (config_.enableLogging) {
        Serial.println("[ConfigurationMidiExtractor ERROR] " + message);
    }
}