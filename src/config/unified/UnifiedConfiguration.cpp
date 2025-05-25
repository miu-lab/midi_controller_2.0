#include "config/unified/UnifiedConfiguration.hpp"
#include <algorithm>
#include <unordered_set>

UnifiedConfiguration::UnifiedConfiguration() {
    controls_.reserve(20);  // Pré-allocation pour éviter réallocations
}

void UnifiedConfiguration::addControl(ControlDefinition control) {
    // Vérifier l'unicité de l'ID
    if (idIndex_.find(control.id) != idIndex_.end()) {
        // Duplicate control ID - ignore in embedded environment
        return;
    }
    
    size_t index = controls_.size();
    controls_.push_back(std::move(control));
    idIndex_[controls_.back().id] = index;
    
    // Si c'est un encodeur avec bouton, indexer aussi l'ID du bouton
    if (controls_.back().hardware.type == InputType::ENCODER && 
        controls_.back().hardware.encoderButtonPin) {
        InputId buttonId = controls_.back().getEncoderButtonId();
        if (buttonId > 0) {
            idIndex_[buttonId] = index;
        }
    }
}

std::unique_ptr<HardwareConfiguration> UnifiedConfiguration::generateHardwareConfig() const {
    auto config = std::make_unique<HardwareConfiguration>();
    
    // Convertir chaque control en InputConfig
    std::vector<InputConfig> configs;
    configs.reserve(controls_.size() * 2);  // Prévoir encodeurs + boutons
    
    for (const auto& control : controls_) {
        if (!control.enabled) continue;
        
        if (control.hardware.type == InputType::ENCODER) {
            // Ajouter l'encodeur
            configs.push_back(toInputConfig(control));
            
            // Ajouter le bouton de l'encodeur si présent
            if (control.hardware.encoderButtonPin) {
                InputConfig buttonConfig = createEncoderButtonInputConfig(control);
                configs.push_back(buttonConfig);
            }
        } else if (control.hardware.type == InputType::BUTTON) {
            configs.push_back(toInputConfig(control));
        }
    }
    
    // Créer HardwareConfiguration et lui passer les configs
    // Note: HardwareConfiguration n'expose pas de méthode pour ajouter des configs
    // On doit donc utiliser la réflexion ou modifier HardwareConfiguration
    // Pour l'instant, on retourne une config vide
    config = std::make_unique<HardwareConfiguration>();
    
    return config;
}

InputConfig UnifiedConfiguration::createEncoderButtonInputConfig(const ControlDefinition& def) const {
    InputConfig config;
    config.id = def.getEncoderButtonId();
    config.name = def.name + "_button";
    config.label = def.label + " Button";
    config.type = InputType::BUTTON;
    config.enabled = def.enabled;
    config.group = def.group;
    config.description = "Button for " + def.description;
    config.displayOrder = def.displayOrder;
    
    // Créer ButtonConfig pour le bouton de l'encodeur
    config.config = toButtonConfig(def, true);
    
    return config;
}

std::unique_ptr<MappingConfiguration> UnifiedConfiguration::generateMappingConfig() const {
    // MappingConfiguration n'expose pas de méthode pour ajouter des mappings
    // On doit retourner la configuration par défaut pour l'instant
    auto config = std::make_unique<MappingConfiguration>();
    
    // TODO: Implémenter une façon de passer les mappings à MappingConfiguration
    // Soit en ajoutant une méthode publique, soit en utilisant une factory friend
    
    return config;
}

std::optional<ControlDefinition> UnifiedConfiguration::findControlById(InputId id) const {
    auto it = idIndex_.find(id);
    if (it != idIndex_.end()) {
        return controls_[it->second];
    }
    return std::nullopt;
}

std::vector<ControlDefinition> UnifiedConfiguration::getControlsByRole(MappingRole role) const {
    std::vector<ControlDefinition> result;
    
    for (const auto& control : controls_) {
        if (control.hasRole(role)) {
            result.push_back(control);
        }
    }
    
    return result;
}

bool UnifiedConfiguration::validate() const {
    // Vérifier l'unicité des IDs
    std::unordered_set<InputId> seenIds;
    
    for (const auto& control : controls_) {
        // Vérifier l'ID principal
        if (control.id == 0) {
            return false;  // ID 0 invalide
        }
        
        if (!seenIds.insert(control.id).second) {
            return false;  // Duplicate ID
        }
        
        // Vérifier l'ID du bouton d'encodeur
        if (control.hardware.type == InputType::ENCODER && 
            control.hardware.encoderButtonPin) {
            InputId buttonId = control.getEncoderButtonId();
            if (!seenIds.insert(buttonId).second) {
                return false;  // Duplicate button ID
            }
        }
        
        // Vérifier que chaque mapping a un rôle valide
        for (const auto& mapping : control.mappings) {
            // MappingRole n'a pas de valeur NONE, on vérifie juste la cohérence
            if (mapping.appliesTo == MappingControlType::ENCODER && 
                control.hardware.type != InputType::ENCODER) {
                return false;
            }
        }
    }
    
    return true;
}

UnifiedConfiguration::Stats UnifiedConfiguration::getStats() const {
    Stats stats{};
    stats.totalControls = controls_.size();
    
    for (const auto& control : controls_) {
        if (control.hardware.type == InputType::ENCODER) {
            stats.encoders++;
            if (control.hardware.encoderButtonPin) {
                stats.buttons++;  // Bouton de l'encodeur
            }
        } else if (control.hardware.type == InputType::BUTTON) {
            stats.buttons++;
        }
        
        for (const auto& mapping : control.mappings) {
            if (mapping.role == MappingRole::MIDI) {
                stats.midiMappings++;
            } else if (mapping.role == MappingRole::NAVIGATION) {
                stats.navigationMappings++;
            }
        }
    }
    
    return stats;
}

// === MÉTHODES PRIVÉES DE CONVERSION ===

InputConfig UnifiedConfiguration::toInputConfig(const ControlDefinition& def) const {
    InputConfig config;
    config.id = def.id;
    config.name = def.name;
    config.label = def.label;
    config.type = def.hardware.type;
    config.enabled = def.enabled;
    config.group = def.group;
    config.description = def.description;
    config.displayOrder = def.displayOrder;
    
    // Configurer le variant selon le type
    if (def.hardware.type == InputType::ENCODER) {
        config.config = toEncoderConfig(def);
    } else {
        config.config = toButtonConfig(def, false);
    }
    
    return config;
}

std::vector<InputMapping> UnifiedConfiguration::toInputMappings(const ControlDefinition& def) const {
    std::vector<InputMapping> mappings;
    
    // Mappings pour le contrôle principal
    for (const auto& mapping : def.mappings) {
        InputMapping im;
        im.controlId = def.id;
        im.mappingType = mapping.appliesTo;
        im.roles = {mapping.role};
        
        if (std::holds_alternative<ControlDefinition::MidiConfig>(mapping.config)) {
            const auto& midi = std::get<ControlDefinition::MidiConfig>(mapping.config);
            im.midiMapping.channel = midi.channel;
            im.midiMapping.control = midi.control;
            im.midiMapping.isRelative = midi.isRelative;
        } else if (std::holds_alternative<ControlDefinition::NavigationConfig>(mapping.config)) {
            const auto& nav = std::get<ControlDefinition::NavigationConfig>(mapping.config);
            im.navigationMapping.name = nav.action;
        }
        
        mappings.push_back(im);
    }
    
    // Mappings pour le bouton de l'encodeur si présent
    if (def.hardware.type == InputType::ENCODER && def.hardware.encoderButtonPin) {
        InputId buttonId = def.getEncoderButtonId();
        
        // Trouver les mappings qui s'appliquent aux boutons
        for (const auto& mapping : def.mappings) {
            if (mapping.appliesTo == MappingControlType::BUTTON) {
                InputMapping im;
                im.controlId = buttonId;
                im.mappingType = MappingControlType::BUTTON;
                im.roles = {mapping.role};
                
                if (std::holds_alternative<ControlDefinition::MidiConfig>(mapping.config)) {
                    const auto& midi = std::get<ControlDefinition::MidiConfig>(mapping.config);
                    im.midiMapping.channel = midi.channel;
                    im.midiMapping.control = midi.control;
                    im.midiMapping.isRelative = midi.isRelative;
                } else if (std::holds_alternative<ControlDefinition::NavigationConfig>(mapping.config)) {
                    const auto& nav = std::get<ControlDefinition::NavigationConfig>(mapping.config);
                    im.navigationMapping.name = nav.action;
                }
                
                mappings.push_back(im);
            }
        }
    }
    
    return mappings;
}

ButtonConfig UnifiedConfiguration::toButtonConfig(const ControlDefinition& def, bool isEncoderButton) const {
    ButtonConfig config;
    
    if (isEncoderButton) {
        // Configuration pour le bouton d'un encodeur
        config.id = def.getEncoderButtonId();
        config.gpio = *def.hardware.encoderButtonPin;
        config.activeLow = true;
        config.mode = ButtonMode::MOMENTARY;
        config.debounceMs = def.hardware.encoderButtonDebounceMs.value_or(30);
        config.enableLongPress = false;
        config.longPressMs = 800;  // Valeur par défaut
    } else {
        // Configuration pour un bouton standalone
        const auto& btnConfig = std::get<ControlDefinition::ButtonConfig>(def.hardware.config);
        
        config.id = def.id;
        config.gpio = btnConfig.pin;
        config.activeLow = btnConfig.activeLow;
        config.mode = btnConfig.mode;
        config.debounceMs = btnConfig.debounceMs;
        if (btnConfig.longPressMs.has_value()) {
            config.longPressMs = *btnConfig.longPressMs;
            config.enableLongPress = true;
        } else {
            config.longPressMs = 800;  // Valeur par défaut
            config.enableLongPress = false;
        }
    }
    
    return config;
}

EncoderConfig UnifiedConfiguration::toEncoderConfig(const ControlDefinition& def) const {
    const auto& encConfig = std::get<ControlDefinition::EncoderConfig>(def.hardware.config);
    
    EncoderConfig config;
    config.id = def.id;
    config.pinA = encConfig.pinA;
    config.pinB = encConfig.pinB;
    config.ppr = encConfig.ppr;
    config.sensitivity = encConfig.sensitivity;
    config.enableAcceleration = encConfig.enableAcceleration;
    config.stepsPerDetent = encConfig.stepsPerDetent;
    config.invertDirection = false;  // Valeur par défaut
    config.accelerationThreshold = 100;  // Valeur par défaut
    config.maxAcceleration = 5.0f;  // Valeur par défaut
    
    // Si le contrôle a un bouton, l'ajouter
    if (def.hardware.encoderButtonPin) {
        ButtonConfig btnConfig;
        btnConfig.id = def.getEncoderButtonId();
        btnConfig.gpio = *def.hardware.encoderButtonPin;
        btnConfig.activeLow = true;
        btnConfig.mode = ButtonMode::MOMENTARY;
        btnConfig.debounceMs = def.hardware.encoderButtonDebounceMs.value_or(30);
        btnConfig.enableLongPress = false;
        btnConfig.longPressMs = 800;
        config.buttonConfig = btnConfig;
    }
    
    return config;
}
