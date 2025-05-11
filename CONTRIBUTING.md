# Guide de Contribution

Merci de votre intérêt pour contribuer au projet MIDI Controller! Ce document vous guidera à travers les étapes pour contribuer efficacement au projet.

## Prérequis

- Connaissance de base de C++ et de la programmation orientée objet
- Familiarité avec PlatformIO et Arduino/Teensy
- Git installé sur votre machine

## Mise en Place de l'Environnement de Développement

1. **Cloner le dépôt**:
   ```bash
   git clone https://github.com/votre-organisation/midi_controller.git
   cd midi_controller
   ```

2. **Installer PlatformIO**:
   Si vous ne l'avez pas déjà, [installez PlatformIO](https://platformio.org/install)

3. **Ouvrir le projet**:
   ```bash
   platformio project open
   ```

## Structure du Projet

Familiarisez-vous avec la structure du projet:

- `src/` - Code source
  - `adapters/` - Adaptateurs pour les interfaces
  - `app/` - Application principale et sous-systèmes
  - `config/` - Configuration
  - `core/` - Logique métier principale
- `test/` - Tests unitaires et d'intégration
- `docs/` - Documentation

## Workflow de Contribution

1. **Créer une branche**:
   ```bash
   git checkout -b feature/ma-nouvelle-fonctionnalite
   ```

2. **Implémenter les changements**:
   Suivez les standards de code et les principes d'architecture décrits dans la documentation.

3. **Exécuter les tests**:
   ```bash
   pio test
   ```

4. **Soumettre un Pull Request**:
   - Assurez-vous que tous les tests passent
   - Fournissez une description claire de vos changements
   - Référencez les issues pertinentes

## Standards de Code

Consultez [CodingStandard.md](CodingStandard.md) pour les détails complets, mais voici les points essentiels:

- Utilisez des suffixes underscore (`_`) pour les membres de classe
- Nommez les classes en PascalCase
- Préfixez les interfaces avec "I"
- Documentez tout le code public avec Doxygen
- Utilisez des smart pointers plutôt que des pointeurs bruts

## Ajouter une Nouvelle Fonctionnalité

### 1. Définir l'Interface

Si votre fonctionnalité nécessite une nouvelle interface, créez-la dans le dossier approprié:

```cpp
// core/domain/interfaces/IMyFeature.hpp
#pragma once

class IMyFeature {
public:
    virtual ~IMyFeature() = default;
    
    virtual void doSomething() = 0;
    virtual int calculateValue(int input) = 0;
};
```

### 2. Implémenter un Sous-système

Si votre fonctionnalité est substantielle, créez un nouveau sous-système:

```cpp
// app/subsystems/MyFeatureSubsystem.hpp
#pragma once

#include <memory>
#include "core/domain/interfaces/IMyFeature.hpp"

class DependencyContainer;

class MyFeatureSubsystem : public IMyFeature {
public:
    explicit MyFeatureSubsystem(std::shared_ptr<DependencyContainer> container);
    ~MyFeatureSubsystem() override = default;
    
    void init();
    void update();
    
    // Implémentation de IMyFeature
    void doSomething() override;
    int calculateValue(int input) override;
    
private:
    std::shared_ptr<DependencyContainer> container_;
    // Autres membres...
};
```

### 3. Mettre à Jour le Conteneur de Dépendances

Modifiez `InitializationScript.hpp` pour inclure votre nouveau sous-système:

```cpp
// Dans initializeContainer
auto myFeature = std::make_shared<MyFeatureSubsystem>(container);
container->registerDependency<MyFeatureSubsystem>(myFeature);
container->registerDependency<IMyFeature>(myFeature);
```

### 4. Ajouter des Tests

Créez des tests pour votre nouvelle fonctionnalité:

```cpp
// test/unit/MyFeatureSubsystemTests.h
#pragma once

#include <unity.h>
#include "app/subsystems/MyFeatureSubsystem.hpp"
#include "mocks/MockDependency.hpp"

void test_my_feature_initialization() {
    // Arrange
    auto container = std::make_shared<DependencyContainer>();
    auto mockDep = std::make_shared<MockDependency>();
    container->registerDependency<IDependency>(mockDep);
    
    // Act
    auto myFeature = std::make_shared<MyFeatureSubsystem>(container);
    myFeature->init();
    
    // Assert
    TEST_ASSERT_TRUE(mockDep->wasInitialized());
}

// Autres tests...
```

## Déboguer sur Teensy

1. **Activer les logs de débogage**:
   Dans `config/debug/DebugConfig.hpp`, activez les options de débogage appropriées.

2. **Déployer et monitorer**:
   ```bash
   pio run -t upload
   pio device monitor
   ```

3. **Analyser les logs**:
   Utilisez les macros de débogage comme `DEBUG_PRINT` et `DEBUG_ERROR` pour tracer l'exécution.

## Questions et Assistance

Si vous avez des questions ou besoin d'aide:
- Consultez la documentation dans le dossier `docs/`
- Ouvrez une issue avec le tag "question"
- Contactez les mainteneurs du projet

Nous apprécions vos contributions et nous nous efforçons de maintenir un environnement collaboratif positif!
