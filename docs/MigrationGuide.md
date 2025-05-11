# Guide de Migration vers la Nouvelle Architecture

Ce document détaille le processus de migration du code existant vers la nouvelle architecture basée sur l'injection de dépendances.

## Introduction

La refactorisation du projet MIDI Controller a été réalisée en plusieurs phases, remplaçant progressivement l'architecture basée sur ServiceLocator par un système moderne d'injection de dépendances. Ce guide explique comment adapter le code existant ou nouveau à cette architecture.

## Les Phases de Refactorisation

Le projet a traversé les phases suivantes:

1. **Phase 1 - Préparation et Infrastructure**: Mise en place des tests et analyse des dépendances
2. **Phase 2 - Système d'Injection de Dépendances**: Implémentation de DependencyContainer
3. **Phase 3 - Création des Sous-systèmes**: Définition des interfaces principales et sous-systèmes
4. **Phase 4 - Modernisation de la Gestion Mémoire**: Remplacement des pointeurs bruts par des smart pointers
5. **Phase 5 - Standardisation du Code**: Normalisation des noms et conventions
6. **Phase 6 - MidiControllerApp2**: Implémentation de la nouvelle application principale
7. **Phase 7 - Migration**: Remplacement progressif de l'ancien code
8. **Phase 8 - Suppression de ServiceLocator**: Élimination complète de l'ancien système
9. **Phase 9 - Injection Directe de Dépendances**: Migration vers l'injection directe
10. **Phase 10 - Finalisation**: Documentation et tests finaux

## Migration des Classes Existantes

### Étape 1: Identifier les Dépendances

Pour chaque classe que vous souhaitez migrer:

1. Identifiez toutes les dépendances (autres classes ou services utilisés)
2. Déterminez quelles interfaces ces dépendances devraient implémenter
3. Notez les cycles de dépendances potentiels

Exemple:
```cpp
// Avant migration
class MyController {
public:
    void process() {
        // Utilisation directe de ServiceLocator
        auto& inputSystem = ServiceLocator::getInputSystem();
        auto& midiSystem = ServiceLocator::getMidiSystem();
        
        // Utilisation des systèmes
        if (inputSystem.hasNewInput()) {
            midiSystem.sendNote(42, 127);
        }
    }
};
```

### Étape 2: Modifier le Constructeur

Modifiez le constructeur pour recevoir les dépendances par injection:

```cpp
// Après migration - Étape 1
class MyController {
public:
    // Injection des dépendances via le constructeur
    MyController(std::shared_ptr<IInputSystem> inputSystem,
                std::shared_ptr<IMidiSystem> midiSystem)
        : inputSystem_(inputSystem),
          midiSystem_(midiSystem) {
    }
    
private:
    std::shared_ptr<IInputSystem> inputSystem_;
    std::shared_ptr<IMidiSystem> midiSystem_;
};
```

### Étape 3: Mettre à Jour l'Utilisation des Dépendances

Remplacez les appels à ServiceLocator par l'utilisation des dépendances injectées:

```cpp
// Après migration - Étape 2
void MyController::process() {
    // Utilisation des dépendances injectées
    if (inputSystem_->hasNewInput()) {
        midiSystem_->sendNote(42, 127);
    }
}
```

### Étape 4: Enregistrer dans le Conteneur de Dépendances

Modifiez le code qui crée votre classe pour utiliser le conteneur de dépendances:

```cpp
// Dans InitializationScript.hpp
auto myController = std::make_shared<MyController>(
    container->resolve<IInputSystem>(),
    container->resolve<IMidiSystem>()
);

container->registerDependency<MyController>(myController);
```

## Migration des Sous-systèmes

Pour les composants majeurs, créez un sous-système qui implémente l'interface correspondante:

```cpp
// Nouveau sous-système
class MySubsystem : public IMySystem {
public:
    explicit MySubsystem(std::shared_ptr<DependencyContainer> container)
        : container_(container) {
    }
    
    Result<bool, std::string> init() override {
        // Résoudre les dépendances
        dependency_ = container_->resolve<IDependency>();
        if (!dependency_) {
            return Result<bool, std::string>("Dépendance manquante");
        }
        
        return Result<bool, std::string>(true);
    }
    
    // Autres méthodes de l'interface
    
private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<IDependency> dependency_;
};
```

## Gestion des Dépendances Circulaires

Pour gérer les dépendances circulaires, plusieurs approches:

### 1. Utilisation d'Interfaces

Dépendre des abstractions plutôt que des implémentations concrètes.

### 2. Utilisation de weak_ptr

```cpp
class ComponentA {
public:
    ComponentA(std::shared_ptr<ComponentB> b) : b_(b) {}
    void setPartner(std::shared_ptr<ComponentA> partner) {
        b_->registerCallback(partner);
    }
private:
    std::shared_ptr<ComponentB> b_;
};

class ComponentB {
public:
    void registerCallback(std::shared_ptr<ComponentA> a) {
        // Utiliser weak_ptr pour éviter les cycles
        a_callback_ = std::weak_ptr<ComponentA>(a);
    }
    
    void doCallback() {
        if (auto a = a_callback_.lock()) {
            // a est valide, on peut l'utiliser
            a->doSomething();
        }
    }
    
private:
    std::weak_ptr<ComponentA> a_callback_;
};
```

### 3. Réorganisation de l'Architecture

Parfois, la meilleure solution est de repenser l'architecture pour éliminer les dépendances circulaires:
- Utiliser le pattern Observer
- Créer un service intermédiaire
- Introduire un bus d'événements

## Checklist de Migration

✅ Identifier et documenter toutes les dépendances  
✅ Créer ou utiliser les interfaces appropriées  
✅ Modifier les constructeurs pour l'injection de dépendances  
✅ Mettre à jour l'utilisation interne des dépendances  
✅ Traiter les dépendances circulaires  
✅ Enregistrer dans le conteneur de dépendances  
✅ Mettre à jour les appelants  
✅ Ajouter des tests pour la nouvelle implémentation  

## Exemples Concrets de Migration

### Exemple 1: Contrôleur Simple

Voir le fichier `examples/ControllerMigration.cpp` pour un exemple complet de migration d'un contrôleur simple.

### Exemple 2: Sous-système Complet

Voir le dossier `examples/subsystem_migration/` pour un exemple détaillé de migration d'un sous-système.

## Conseils et Pièges à Éviter

- **Ne mélangez pas** les styles: évitez d'avoir certaines dépendances injectées et d'autres récupérées via ServiceLocator
- **Testez au fur et à mesure**: ne migrez pas trop de code à la fois
- **Utilisez les smart pointers de manière cohérente**:
  - `shared_ptr` pour les dépendances partagées
  - `unique_ptr` pour les composants appartenant exclusivement à leur parent
  - `weak_ptr` pour briser les cycles de dépendances
- **Respectez les conventions de nommage** définies dans le projet
