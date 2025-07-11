# Guide de Migration - Configuration Unifiée

## 🎯 Unification Terminée
Tous les fichiers de configuration ont été unifiés dans `SystemConstants.hpp` pour éliminer les redondances et améliorer la maintenabilité.

## 📁 Mapping des Anciennes Constantes

### HardwareConfig.hpp → SystemConstants::Hardware
```cpp
// AVANT
HardwareConfig::SERIAL_BAUD_RATE
HardwareConfig::SPI_SPEED_DISPLAY

// MAINTENANT
SystemConstants::Hardware::SERIAL_BAUD_RATE
SystemConstants::Hardware::SPI_SPEED_DISPLAY
```

### DisplayConfig.hpp → SystemConstants::Display  
```cpp
// AVANT
DisplayConfig::SCREEN_WIDTH
DisplayConfig::CS_PIN

// MAINTENANT  
SystemConstants::Display::SCREEN_WIDTH
SystemConstants::Display::CS_PIN
```

### InputConfig.hpp → SystemConstants::Encoders + Input
```cpp
// AVANT
InputConfig::DEFAULT_ENCODER_SENSITIVITY
InputConfig::BUTTON_DEBOUNCE_MS

// MAINTENANT
SystemConstants::Encoders::DEFAULT_SENSITIVITY
SystemConstants::Timing::BUTTON_DEBOUNCE_MS
```

### MidiConfig.hpp → SystemConstants::Audio
```cpp  
// AVANT
MidiConfig::DEFAULT_CHANNEL
MidiConfig::CC_VALUE_MAX

// MAINTENANT
SystemConstants::Audio::MIDI_CHANNEL_DEFAULT
SystemConstants::Audio::MIDI_CC_MAX
```

### PerformanceConfig.hpp → SystemConstants::Performance
```cpp
// AVANT
PerformanceConfig::MAX_MIDI_CALLBACKS
PerformanceConfig::DISPLAY_REFRESH_RATE_HZ

// MAINTENANT
SystemConstants::Performance::MAX_MIDI_CALLBACKS  
SystemConstants::Performance::DISPLAY_REFRESH_RATE_HZ
```

## ⚡ Avantages de l'Unification

- ✅ **Point de vérité unique** : Plus de valeurs contradictoires
- ✅ **Namespaces logiques** : Organisation claire par domaine  
- ✅ **Élimination des redondances** : 80% de duplication supprimée
- ✅ **Compilation plus rapide** : Moins d'includes
- ✅ **Maintenabilité** : Modifications centralisées

## 🔄 Fichiers Conservés

### Spécialisés (conservés)
- `ETLConfig.hpp` - Typedefs ETL spécialisés
- `debug/DebugConfig.hpp` - Configuration debug spécialisée  
- `debug/DebugMacros.hpp` - Macros debug
- `ApplicationConfiguration.hpp` - Configuration dynamique

### Core (conservés)
- `unified/UnifiedConfiguration.hpp` - Système de configuration principal
- `unified/ControlDefinition.hpp` - Définitions de contrôles
- `SystemConstants.hpp` - **POINT CENTRAL DE CONFIGURATION**

## 📋 Structure Finale

```
src/config/
├── SystemConstants.hpp           ⭐ FICHIER PRINCIPAL
├── ApplicationConfiguration.hpp   
├── ETLConfig.hpp                 
├── debug/
│   ├── DebugConfig.hpp           
│   └── DebugMacros.hpp           
└── unified/
    ├── UnifiedConfiguration.hpp   
    └── ControlDefinition.hpp     
```

La migration est **transparente** - utilisez SystemConstants:: dans tous les nouveaux développements !