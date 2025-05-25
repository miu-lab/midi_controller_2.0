# Option 3 : Adapter l'Architecture (RECOMMANDÉ)

## Utiliser UnifiedConfiguration directement sans passer par les anciennes classes

### 1. Modifier ConfigurationSubsystem pour exposer UnifiedConfiguration
```cpp
// ConfigurationSubsystem.hpp
class ConfigurationSubsystem : public IConfiguration {
private:
    std::unique_ptr<UnifiedConfiguration> unifiedConfig_;
    
    // Garder pour compatibilité temporaire
    std::unique_ptr<HardwareConfiguration> hardwareConfig_;
    std::unique_ptr<MappingConfiguration> mappingConfig_;

public:
    void init() override {
        // Créer la configuration unifiée
        unifiedConfig_ = ConfigurationFactory::createDefaultConfiguration();
        
        // Pour compatibilité, créer aussi les anciennes (vides pour l'instant)
        hardwareConfig_ = std::make_unique<HardwareConfiguration>();
        mappingConfig_ = std::make_unique<MappingConfiguration>();
    }
    
    // NOUVELLES MÉTHODES
    std::optional<InputConfig> getInputConfig(InputId id) const {
        if (unifiedConfig_) {
            if (auto control = unifiedConfig_->findControlById(id)) {
                return toInputConfig(*control);
            }
        }
        return std::nullopt;
    }
    
    std::vector<InputConfig> getAllInputConfigs() const {
        std::vector<InputConfig> configs;
        if (unifiedConfig_) {
            // Convertir tous les ControlDefinition en InputConfig
            auto stats = unifiedConfig_->getStats();
            configs.reserve(stats.totalControls);
            
            // Logique de conversion...
        }
        return configs;
    }
    
    const MidiControl* getMidiMapping(InputId id, MappingControlType type) const {
        if (unifiedConfig_) {
            if (auto control = unifiedConfig_->findControlById(id)) {
                // Extraire le mapping MIDI depuis ControlDefinition
                for (const auto& mapping : control->mappings) {
                    if (mapping.role == MappingRole::MIDI && 
                        mapping.appliesTo == type) {
                        // Retourner les infos MIDI
                    }
                }
            }
        }
        // Fallback sur l'ancienne méthode
        return mappingConfig_->getMidiMappingForControl(id, type);
    }
};
```

### 2. Adapter progressivement les consommateurs
```cpp
// InputSubsystem peut progressivement utiliser les nouvelles méthodes
void InputSubsystem::init() {
    const auto& config = core_->getSubsystem<IConfiguration>();
    
    // Nouvelle approche
    auto inputConfigs = config.getAllInputConfigs();
    for (const auto& inputConfig : inputConfigs) {
        // Traiter chaque config...
    }
    
    // Ou approche hybride pendant la transition
    if (auto unified = dynamic_cast<ConfigurationSubsystem*>(&config)) {
        // Utiliser les nouvelles méthodes
    } else {
        // Utiliser l'ancienne approche
    }
}
```

### 3. Exemple d'implémentation complète
```cpp
// ConfigurationSubsystem.cpp
class ConfigurationSubsystem : public IConfiguration {
private:
    // Méthode helper pour convertir ControlDefinition en InputConfig
    InputConfig toInputConfig(const ControlDefinition& def) const {
        InputConfig config;
        config.id = def.id;
        config.name = def.name;
        config.label = def.label;
        config.type = def.hardware.type;
        config.enabled = def.enabled;
        config.group = def.group;
        config.description = def.description;
        config.displayOrder = def.displayOrder;
        
        if (def.hardware.type == InputType::ENCODER) {
            const auto& enc = std::get<ControlDefinition::EncoderConfig>(def.hardware.config);
            EncoderConfig encoderConfig;
            encoderConfig.id = def.id;
            encoderConfig.pinA = enc.pinA;
            encoderConfig.pinB = enc.pinB;
            encoderConfig.ppr = enc.ppr;
            encoderConfig.sensitivity = enc.sensitivity;
            encoderConfig.enableAcceleration = enc.enableAcceleration;
            encoderConfig.stepsPerDetent = enc.stepsPerDetent;
            
            // Ajouter le bouton si présent
            if (def.hardware.encoderButtonPin) {
                ButtonConfig btnConfig;
                btnConfig.id = def.getEncoderButtonId();
                btnConfig.gpio = *def.hardware.encoderButtonPin;
                btnConfig.activeLow = true;
                btnConfig.mode = ButtonMode::MOMENTARY;
                btnConfig.debounceMs = def.hardware.encoderButtonDebounceMs.value_or(30);
                encoderConfig.buttonConfig = btnConfig;
            }
            
            config.config = encoderConfig;
        } else {
            const auto& btn = std::get<ControlDefinition::ButtonConfig>(def.hardware.config);
            ButtonConfig buttonConfig;
            buttonConfig.id = def.id;
            buttonConfig.gpio = btn.pin;
            buttonConfig.activeLow = btn.activeLow;
            buttonConfig.mode = btn.mode;
            buttonConfig.debounceMs = btn.debounceMs;
            buttonConfig.longPressMs = btn.longPressMs.value_or(800);
            buttonConfig.enableLongPress = btn.longPressMs.has_value();
            
            config.config = buttonConfig;
        }
        
        return config;
    }
};
```

## Avantages :
- ✅ Pas de modification des classes existantes
- ✅ Migration progressive possible
- ✅ Architecture plus propre à terme
- ✅ Moins de code de conversion
- ✅ API moderne accessible immédiatement
- ✅ Possibilité de supprimer l'ancien code plus tard

## Inconvénients :
- ❌ Nécessite d'adapter ConfigurationSubsystem
- ❌ Migration progressive des consommateurs
- ❌ Duplication temporaire pendant la transition

## Plan de Migration :
1. **Phase 1** : Ajouter UnifiedConfiguration à ConfigurationSubsystem
2. **Phase 2** : Ajouter nouvelles méthodes qui utilisent UnifiedConfiguration
3. **Phase 3** : Migrer progressivement les consommateurs
4. **Phase 4** : Supprimer les anciennes classes HardwareConfiguration et MappingConfiguration
5. **Phase 5** : Nettoyer et optimiser

## Exemple de test rapide :
```cpp
// Dans main.cpp ou un test
void testUnifiedConfig() {
    auto config = ConfigurationFactory::createDefaultConfiguration();
    
    // Tester les requêtes
    auto stats = config->getStats();
    Serial.printf("Total controls: %d\n", stats.totalControls);
    Serial.printf("Encoders: %d, Buttons: %d\n", stats.encoders, stats.buttons);
    
    // Tester la recherche
    if (auto encoder1 = config->findControlById(71)) {
        Serial.printf("Found encoder 1: %s\n", encoder1->label.c_str());
    }
    
    // Tester par rôle
    auto midiControls = config->getControlsByRole(MappingRole::MIDI);
    Serial.printf("MIDI controls: %d\n", midiControls.size());
}
```
