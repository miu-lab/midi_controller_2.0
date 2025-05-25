# Inventaire des Usages - Configuration System

## HardwareConfiguration - Points d'Utilisation

### 1. ConfigurationSubsystem

**Fichier**: `src/app/subsystems/ConfigurationSubsystem.cpp`

**Utilisations**:
```cpp
// Ligne 11 - Inclusion
#include "config/HardwareConfiguration.hpp"

// Ligne 15 - Création
hardwareConfig_ = std::make_shared<HardwareConfiguration>();

// Ligne 41-56 - Exposition via IConfiguration
const std::vector<InputConfig>& getAllInputConfigurations() const {
    return hardwareConfig_->getAllInputConfigurations();
}

std::vector<InputConfig> getInputConfigurationsByType(InputType type) const {
    return hardwareConfig_->getInputConfigurationsByType(type);
}

std::optional<InputConfig> getInputConfigurationById(InputId id) const {
    return hardwareConfig_->getInputConfigurationById(id);
}

// Ligne 107 - Validation
bool validateAllConfigurations() const {
    return hardwareConfig_->validateAllConfigurations();
}
```

### 2. InputSubsystem (Indirect)

**Fichier**: `src/app/subsystems/InputSubsystem.cpp`

**Utilisations** (via IConfiguration):
```cpp
// Ligne 36-37 - Résolution de la configuration
configuration_ = container_->resolve<IConfiguration>();

// Ligne 115 - Obtention des configurations
const auto& allInputConfigs = configuration_->getAllInputConfigurations();

// Ligne 121 - Validation
if (!configuration_->validateAllConfigurations()) {

// Ligne 163 - Requête par ID
return configuration_->getInputConfigurationById(id);
```

## MappingConfiguration - Points d'Utilisation

### 1. MidiSubsystem

**Fichier**: `src/app/subsystems/MidiSubsystem.cpp`

**Utilisations**:
```cpp
// Ligne 45 - Création directe
mappingConfig_ = std::make_shared<MappingConfiguration>();

// Ligne 67 - Passage au MidiMapper
midiMapper_ = std::make_unique<MidiMapper>(*midiOut_, *mappingConfig_);
```

### 2. MidiMapper

**Fichier**: `src/adapters/secondary/midi/MidiMapper.cpp`

**Utilisations**:
```cpp
// Constructeur - Stockage de la référence
MidiMapper(IMidiOut& midiOut, const MappingConfiguration& mappingConfig)
    : midiOut_(midiOut), mappingConfig_(mappingConfig)

// Ligne 84 - Recherche de mapping MIDI
auto midiControl = mappingConfig_.getMidiMappingForControl(
    encoderEvent.encoderId, MappingControlType::ENCODER);

// Ligne 127 - Recherche pour bouton
auto midiControl = mappingConfig_.getMidiMappingForControl(
    buttonId, MappingControlType::BUTTON);

// Ligne 169 - Vérification navigation
bool isNavigation = mappingConfig_.isNavigationControl(controlId);
```

### 3. InputController (Indirect)

**Fichier**: `src/core/controllers/InputController.cpp`

**Utilisations** (via NavigationConfigService):
```cpp
// Ligne 45 - Vérification si contrôle de navigation
if (navConfig_->isNavigationControl(encoderId)) {

// Ligne 78 - Vérification pour bouton
if (navConfig_->isNavigationControl(buttonId)) {
```

**NavigationConfigService** encapsule MappingConfiguration:
```cpp
// src/app/services/NavigationConfigService.cpp
mappingConfig_ = std::make_shared<MappingConfiguration>();

bool isNavigationControl(InputId controlId) const {
    return mappingConfig_->isNavigationControl(controlId);
}
```

## Conventions Implicites Documentées

### 1. Convention ID 1000+

**Où elle est définie**:
- Jamais explicitement documentée dans le code
- Appliquée dans `HardwareConfiguration::setInputsConfiguration()`
- Respectée dans `MappingConfiguration::setupMapping()`

**Exemples**:
```cpp
// HardwareConfiguration.cpp
EncoderConfig encoder1{
    .id = 71,
    .buttonConfig = ButtonConfig{
        .id = 1071,  // <-- 71 + 1000
        ...
    }
};

// MappingConfiguration.cpp
controlsMapping.emplace_back(
    InputMapping{.controlId = 71,    // Encodeur
                 .mappingType = MappingControlType::ENCODER...});
controlsMapping.emplace_back(
    InputMapping{.controlId = 1071,  // Bouton encodeur
                 .mappingType = MappingControlType::BUTTON...});
```

### 2. Convention des Groupes

**Groupes utilisés**:
- `"MIDI"` : Encodeurs 71-78
- `"Navigation"` : Encodeur 79, Boutons 51-52
- `"Precision"` : Encodeur 80

### 3. Convention Display Order

**Calcul actuel**:
```cpp
// Pour les boutons
.displayOrder = static_cast<uint8_t>(id - 50)

// Pour les encodeurs
.displayOrder = static_cast<uint8_t>(id - 70)
```

## Dépendances Externes

### 1. EncoderManager

**Création dans InputSubsystem**:
```cpp
encoderManager_ = std::make_shared<EncoderManager>(encoderConfigs);
```

**Attend**: `std::vector<EncoderConfig>`

### 2. ButtonManager

**Création dans InputSubsystem**:
```cpp
buttonManager_ = std::make_shared<DigitalButtonManager>(buttonConfigs);
```

**Attend**: `std::vector<ButtonConfig>`

### 3. ProcessEncoders / ProcessButtons

**Utilisent les managers**:
```cpp
processEncoders_ = std::make_unique<ProcessEncoders>(encoderManager_->getEncoders());
processButtons_ = std::make_unique<ProcessButtons>(buttonManager_->getButtons());
```

## Résumé des Contraintes pour la Migration

1. **Compatibilité Binaire**
   - Les structures `InputConfig`, `EncoderConfig`, `ButtonConfig` doivent rester identiques
   - Les IDs doivent être exactement les mêmes

2. **Ordre de Création**
   - ConfigurationSubsystem doit être initialisé avant InputSubsystem
   - MappingConfiguration est créé indépendamment dans MidiSubsystem

3. **Points d'Injection**
   - ConfigurationSubsystem : Remplacer `new HardwareConfiguration()` par génération depuis Unified
   - MidiSubsystem : Remplacer `new MappingConfiguration()` par génération depuis Unified
   - NavigationConfigService : Potentiellement utiliser UnifiedConfiguration directement

4. **Validation Requise**
   - Tous les IDs de HardwareConfiguration doivent avoir un mapping
   - La convention 1000+ doit être respectée pour les boutons d'encodeurs
   - Les groupes et ordres d'affichage doivent être préservés
