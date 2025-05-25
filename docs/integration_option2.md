# Option 2 : Héritage et Extension

## Créer des classes dérivées :

### 1. ExtendedHardwareConfiguration.hpp
```cpp
#pragma once
#include "config/HardwareConfiguration.hpp"

class ExtendedHardwareConfiguration : public HardwareConfiguration {
public:
    ExtendedHardwareConfiguration(std::vector<InputConfig> configs) {
        // Si inputConfigurations_ est protected, on peut y accéder
        // Sinon, on doit utiliser une autre approche
    }
    
    // Ou utiliser une factory static
    static std::unique_ptr<HardwareConfiguration> 
    createFromInputConfigs(std::vector<InputConfig> configs);
};
```

### 2. ExtendedMappingConfiguration.hpp
```cpp
#pragma once
#include "config/MappingConfiguration.hpp"

class ExtendedMappingConfiguration : public MappingConfiguration {
public:
    ExtendedMappingConfiguration(std::vector<InputMapping> mappings);
    
    // Méthode pour construire avec les mappings
    void initializeWithMappings(std::vector<InputMapping> mappings);
};
```

## Avantages :
- ✅ Ne modifie pas les classes originales
- ✅ Peut ajouter des fonctionnalités
- ✅ Compatible avec l'existant

## Inconvénients :
- ❌ Nécessite des membres protected (pas private)
- ❌ Ajoute de la complexité
- ❌ Peut nécessiter des modifications mineures
