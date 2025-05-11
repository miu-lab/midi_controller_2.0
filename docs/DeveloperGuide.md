# Guide du développeur MIDI Controller

Ce document explique comment développer et étendre l'application MIDI Controller.

## Architecture générale

L'application utilise une architecture modulaire basée sur l'injection de dépendances:

```
                   +-------------------+
                   | MidiControllerApp |
                   +-------------------+
                            |
                            v
            +-----------------------------------+
            |       DependencyContainer         |
            +-----------------------------------+
                |         |         |         |
                v         v         v         v
        +------------+  +------+  +-------+  +------+
        |    Config  |  | Input |  | MIDI  |  |  UI  |
        +------------+  +------+  +-------+  +------+
```

## Ajout d'un nouveau composant

Pour ajouter un nouveau composant à l'application, suivez ces étapes:

### 1. Définir l'interface

Créez d'abord une interface pour votre composant dans le dossier `core/domain/interfaces/`:

```cpp
// INewFeature.hpp
#pragma once

#include "core/utils/Result.hpp"

class INewFeature {
public:
    virtual ~INewFeature() = default;
    
    virtual Result<bool, std::string> init() = 0;
    virtual void update() = 0;
    
    // Ajoutez ici les méthodes spécifiques à votre fonctionnalité
};
```

### 2. Implémenter la classe

Implémentez l'interface dans un sous-système approprié:

```cpp
// NewFeatureSubsystem.hpp
#pragma once

#include <memory>
#include "core/domain/interfaces/INewFeature.hpp"
#include "app/di/DependencyContainer.hpp"

class NewFeatureSubsystem : public INewFeature {
public:
    explicit NewFeatureSubsystem(std::shared_ptr<DependencyContainer> container);
    ~NewFeatureSubsystem() override = default;
    
    Result<bool, std::string> init() override;
    void update() override;
    
private:
    std::shared_ptr<DependencyContainer> container_;
    // Autres membres privés...
};
```

```cpp
// NewFeatureSubsystem.cpp
#include "NewFeatureSubsystem.hpp"

NewFeatureSubsystem::NewFeatureSubsystem(std::shared_ptr<DependencyContainer> container)
    : container_(container) {
}

Result<bool, std::string> NewFeatureSubsystem::init() {
    // Implémenter l'initialisation
    return Result<bool, std::string>::success(true);
}

void NewFeatureSubsystem::update() {
    // Implémenter la mise à jour
}
```

### 3. Enregistrer le composant dans le conteneur

Modifiez `InitializationScript.hpp` pour enregistrer votre nouveau composant:

```cpp
// Dans InitializationScript.hpp
static bool initializeContainer(std::shared_ptr<DependencyContainer> container, 
                               const ApplicationConfiguration& config) {
    // Code existant...
    
    // Ajouter le nouveau sous-système
    auto newFeature = std::make_shared<NewFeatureSubsystem>(container);
    container->registerDependency<NewFeatureSubsystem>(newFeature);
    container->registerDependency<INewFeature>(newFeature);
    
    return true;
}
```

### 4. Modifier MidiControllerApp pour utiliser le nouveau composant

Mettez à jour `MidiControllerApp` pour utiliser votre nouveau composant:

```cpp
// Dans MidiControllerApp.hpp
class MidiControllerApp {
public:
    // Code existant...
    
private:
    // Code existant...
    std::shared_ptr<INewFeature> m_newFeature;
};
```

```cpp
// Dans MidiControllerApp.cpp
Result<bool, std::string> MidiControllerApp::init() {
    // Code existant...
    
    // Résoudre le nouveau composant
    m_newFeature = m_container->resolve<INewFeature>();
    if (!m_newFeature) {
        return Result<bool, std::string>::error("Failed to resolve INewFeature");
    }
    
    // Initialiser le nouveau composant
    auto newFeatureResult = m_newFeature->init();
    if (newFeatureResult.isError()) {
        return Result<bool, std::string>::error(
            std::string("Failed to initialize new feature: ") + 
            *(newFeatureResult.error()));
    }
    
    return Result<bool, std::string>::success(true);
}

void MidiControllerApp::update() {
    // Code existant...
    
    // Mettre à jour le nouveau composant
    if (m_newFeature) m_newFeature->update();
}
```

### 5. Créer des tests pour le nouveau composant

Créez des tests unitaires pour votre composant:

```cpp
// Dans tests/unit/NewFeatureTests.cpp
#include "../TestFramework.hpp"
#include "app/subsystems/NewFeatureSubsystem.hpp"

bool testNewFeatureSubsystem(std::string& errorMessage) {
    auto container = std::make_shared<DependencyContainer>();
    
    // Configurer le conteneur avec les dépendances requises
    
    // Créer le sous-système
    auto newFeature = std::make_shared<NewFeatureSubsystem>(container);
    TEST_ASSERT_NOT_NULL(newFeature, "Failed to create NewFeatureSubsystem");
    
    // Initialiser le sous-système
    auto result = newFeature->init();
    if (result.isError()) {
        errorMessage = "NewFeatureSubsystem initialization failed: " + *result.error();
        return false;
    }
    
    // Tester les méthodes principales
    newFeature->update(); // Devrait fonctionner sans erreur
    
    return true;
}
```

## Bonnes pratiques

### Noms des membres

Utilisez un préfixe cohérent pour les membres de la classe:

```cpp
class MyClass {
private:
    int m_count;         // Préfixe m_
    std::string m_name;  // Préfixe m_
};
```

### Documentation

Documentez toutes les classes et méthodes publiques avec des commentaires Doxygen:

```cpp
/**
 * @brief Description courte
 *
 * Description détaillée si nécessaire
 *
 * @param param1 Description du premier paramètre
 * @param param2 Description du deuxième paramètre
 * @return Description de la valeur de retour
 */
Result<bool, std::string> myMethod(int param1, std::string param2);
```

### Gestion des erreurs

Utilisez le pattern `Result<T, E>` pour la gestion des erreurs:

```cpp
Result<bool, std::string> doSomething() {
    if (/* erreur */) {
        return Result<bool, std::string>::error("Description de l'erreur");
    }
    return Result<bool, std::string>::success(true);
}
```

### Tests unitaires

Écrivez des tests unitaires pour chaque nouveau composant:

```cpp
bool testMyComponent(std::string& errorMessage) {
    // Configurer le test
    
    // Exécuter l'opération à tester
    
    // Vérifier les résultats
    TEST_ASSERT(condition, "Description de l'échec");
    
    return true;
}
```

## Troubleshooting

### Erreurs courantes

#### 1. Dépendance non résolue

```
Failed to resolve IDependency
```

**Solution**: Vérifiez que la dépendance est correctement enregistrée dans `InitializationScript`.

#### 2. Échec d'initialisation

```
Failed to initialize component: Specific error message
```

**Solution**: Vérifiez les dépendances du composant et assurez-vous qu'elles sont toutes correctement initialisées.

#### 3. Erreurs de compilation avec smart pointers

```
error: no matching function for call to 'std::shared_ptr<Interface>::shared_ptr(Implementation*)'
```

**Solution**: Utilisez `std::static_pointer_cast<Interface>(std::make_shared<Implementation>())` pour convertir correctement les types.
