# Guide d'Intégration de la Configuration Unifiée

## 1. Intégration Minimale (Phase 3.1)

### Option A : Dans ConfigurationSubsystem (Recommandé)

```cpp
// src/subsystems/configuration/ConfigurationSubsystem.cpp

#include "config/unified/ConfigurationFactory.hpp"

void ConfigurationSubsystem::init() {
    if (initialized_) return;

    try {
        // NOUVEAU : Utiliser la configuration unifiée
        auto unifiedConfig = ConfigurationFactory::createDefaultConfiguration();
        
        // Générer les configurations compatibles
        hardwareConfig_ = unifiedConfig->generateHardwareConfig();
        mappingConfig_ = unifiedConfig->generateMappingConfig();
        
        // Garder une référence pour usage futur (optionnel)
        unifiedConfig_ = std::move(unifiedConfig);
        
        initialized_ = true;
        ESP_LOGI(TAG, "Configuration initialized successfully");
        
    } catch (const std::exception& e) {
        ESP_LOGE(TAG, "Failed to initialize configuration: %s", e.what());
        // Fallback sur l'ancienne méthode si nécessaire
        hardwareConfig_ = std::make_unique<HardwareConfiguration>();
        mappingConfig_ = std::make_unique<MappingConfiguration>();
    }
}
```

### Option B : Factory Method

```cpp
// Ajouter une méthode factory dans ConfigurationSubsystem
std::unique_ptr<IConfiguration> ConfigurationSubsystem::createUnifiedConfiguration() {
    auto unified = ConfigurationFactory::createDefaultConfiguration();
    
    // Stocker les configs générées
    hardwareConfig_ = unified->generateHardwareConfig();
    mappingConfig_ = unified->generateMappingConfig();
    
    return std::make_unique<ConfigurationSubsystem>();
}
```

## 2. Tests d'Intégration (Phase 3.2)

### Test de Compatibilité

```cpp
// test/integration/test_unified_integration.cpp

void test_unified_config_compatibility() {
    // Créer les deux configurations
    auto oldHwConfig = std::make_unique<HardwareConfiguration>();
    auto oldMapConfig = std::make_unique<MappingConfiguration>();
    
    auto unified = ConfigurationFactory::createDefaultConfiguration();
    auto newHwConfig = unified->generateHardwareConfig();
    auto newMapConfig = unified->generateMappingConfig();
    
    // Comparer les encodeurs
    auto oldEncoders = oldHwConfig->getEncoders();
    auto newEncoders = newHwConfig->getEncoders();
    
    TEST_ASSERT_EQUAL(oldEncoders.size(), newEncoders.size());
    
    // Vérifier chaque encodeur
    for (size_t i = 0; i < oldEncoders.size(); ++i) {
        TEST_ASSERT_EQUAL(oldEncoders[i].id, newEncoders[i].id);
        TEST_ASSERT_EQUAL(oldEncoders[i].pinA.pin, newEncoders[i].pinA.pin);
        TEST_ASSERT_EQUAL(oldEncoders[i].pinB.pin, newEncoders[i].pinB.pin);
        TEST_ASSERT_EQUAL(oldEncoders[i].ppr, newEncoders[i].ppr);
    }
    
    // Comparer les mappings
    auto oldMappings = oldMapConfig->getMappedControls();
    auto newMappings = newMapConfig->getMappedControls();
    
    TEST_ASSERT_EQUAL(oldMappings.size(), newMappings.size());
}
```

### Test de Performance

```cpp
void test_unified_config_performance() {
    unsigned long startOld = micros();
    auto oldHw = std::make_unique<HardwareConfiguration>();
    auto oldMap = std::make_unique<MappingConfiguration>();
    unsigned long timeOld = micros() - startOld;
    
    unsigned long startNew = micros();
    auto unified = ConfigurationFactory::createDefaultConfiguration();
    auto newHw = unified->generateHardwareConfig();
    auto newMap = unified->generateMappingConfig();
    unsigned long timeNew = micros() - startNew;
    
    Serial.printf("Old config time: %lu µs\n", timeOld);
    Serial.printf("New config time: %lu µs\n", timeNew);
    Serial.printf("Difference: %ld µs (%+.1f%%)\n", 
                  (long)timeNew - (long)timeOld,
                  100.0 * ((float)timeNew - (float)timeOld) / (float)timeOld);
}
```

## 3. Migration Progressive (Phase 3.3)

### Étape 1 : Ajouter l'API Unifiée à IConfiguration

```cpp
// src/core/interfaces/IConfiguration.hpp

class IConfiguration {
public:
    // API existante...
    
    // NOUVEAU : API Unifiée (optionnelle au début)
    virtual std::optional<ControlDefinition> getControlDefinition(InputId id) const {
        // Par défaut, retourne nullopt pour rétrocompatibilité
        return std::nullopt;
    }
    
    virtual std::vector<ControlDefinition> getControlsByRole(MappingRole role) const {
        // Par défaut, retourne vecteur vide
        return {};
    }
};
```

### Étape 2 : Implémenter dans ConfigurationSubsystem

```cpp
// src/subsystems/configuration/ConfigurationSubsystem.hpp

class ConfigurationSubsystem : public IConfiguration {
private:
    // NOUVEAU : Garder la config unifiée
    std::unique_ptr<UnifiedConfiguration> unifiedConfig_;
    
public:
    // Implémenter la nouvelle API
    std::optional<ControlDefinition> getControlDefinition(InputId id) const override {
        if (unifiedConfig_) {
            return unifiedConfig_->findControlById(id);
        }
        return std::nullopt;
    }
};
```

### Étape 3 : Migrer les Consommateurs

```cpp
// Exemple : MidiSubsystem utilisant la nouvelle API

void MidiSubsystem::processInput(const InputEvent& event) {
    // Ancienne méthode (toujours fonctionnelle)
    auto mapping = mappingConfig_->getMidiMappingForControl(
        event.sourceId, event.sourceType
    );
    
    // NOUVELLE méthode (plus riche en infos)
    if (auto control = config_->getControlDefinition(event.sourceId)) {
        // Accès à toutes les infos du contrôle
        Serial.printf("Processing %s (group: %s)\n", 
                     control->label.c_str(), 
                     control->group.c_str());
        
        // Traiter les mappings
        for (const auto& mapping : control->getMappingsForRole(MappingRole::MIDI)) {
            // ...
        }
    }
}
```

## 4. Configuration Dynamique (Futur)

### Chargement depuis JSON

```cpp
// Future extension possible
class JsonConfigurationLoader {
public:
    static std::unique_ptr<UnifiedConfiguration> loadFromJson(const char* json) {
        auto config = std::make_unique<UnifiedConfiguration>();
        
        // Parser le JSON avec ArduinoJson
        DynamicJsonDocument doc(4096);
        deserializeJson(doc, json);
        
        // Créer les contrôles
        for (const auto& control : doc["controls"].as<JsonArray>()) {
            ControlBuilder builder(
                control["id"].as<InputId>(),
                control["name"].as<const char*>()
            );
            
            // Configurer selon le type
            if (control["type"] == "encoder") {
                builder.asEncoder(
                    control["pinA"].as<uint8_t>(),
                    control["pinB"].as<uint8_t>(),
                    control["ppr"].as<uint16_t>()
                );
            }
            // ... etc
            
            config->addControl(builder.build());
        }
        
        return config;
    }
};
```

### Hot Reload

```cpp
// Recharger la configuration sans redémarrer
void ConfigurationSubsystem::reloadConfiguration() {
    // Sauvegarder l'ancienne config
    auto oldUnified = std::move(unifiedConfig_);
    
    try {
        // Charger la nouvelle
        unifiedConfig_ = ConfigurationFactory::createDefaultConfiguration();
        
        // Régénérer
        hardwareConfig_ = unifiedConfig_->generateHardwareConfig();
        mappingConfig_ = unifiedConfig_->generateMappingConfig();
        
        // Notifier les observateurs
        notifyConfigurationChanged();
        
    } catch (const std::exception& e) {
        // Restaurer l'ancienne en cas d'erreur
        unifiedConfig_ = std::move(oldUnified);
        ESP_LOGE(TAG, "Failed to reload config: %s", e.what());
    }
}
```

## 5. Checklist de Migration

- [ ] Backup du code actuel
- [ ] Intégration dans ConfigurationSubsystem
- [ ] Tests unitaires passent
- [ ] Tests d'intégration passent
- [ ] Test sur hardware réel
- [ ] Benchmark performance
- [ ] Documentation mise à jour
- [ ] Code review avec l'équipe
- [ ] Planifier suppression ancien code

## 6. Rollback Plan

Si problème détecté :

1. **Immédiat** : Le try/catch dans `init()` utilise l'ancienne config
2. **Court terme** : Désactiver avec un flag
   ```cpp
   #define USE_UNIFIED_CONFIG 0  // Désactiver temporairement
   ```
3. **Debug** : Logs détaillés pour identifier le problème
4. **Fix** : Corriger et re-tester

## Conclusion

Cette approche permet une migration en douceur :
- ✅ Aucune rupture de compatibilité
- ✅ Rollback facile si nécessaire
- ✅ Migration progressive possible
- ✅ Bénéfices immédiats (moins de code)
- ✅ Extensibilité future (JSON, hot-reload)
