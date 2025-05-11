# Implémentation du Système d'Injection de Dépendances

## Vue d'ensemble

Pour améliorer la structure de notre application et éliminer les dépendances globales problématiques, nous avons implémenté un système d'injection de dépendances moderne. Ce système remplace progressivement le `ServiceLocator` existant qui créait un couplage fort entre les composants.

## DependencyContainer

### Concept

Le `DependencyContainer` est une classe qui centralise la gestion des dépendances entre composants. Il permet :

- D'enregistrer des instances d'objets
- De récupérer ces instances quand nécessaire
- De créer des instances à la demande via des factories
- D'enregistrer des implémentations d'interfaces

### Avantages par rapport au ServiceLocator

1. **Dépendances explicites** : Les dépendances sont clairement visibles dans les constructeurs
2. **Meilleure testabilité** : Facilité pour injecter des mocks dans les tests
3. **Cycle de vie géré** : Utilisation de smart pointers pour une gestion automatique
4. **Pas de singleton global** : Élimine les dépendances globales cachées
5. **Flexibilité accrue** : Support pour différentes stratégies d'enregistrement

### Interface principale

```cpp
class DependencyContainer {
public:
    // Enregistre une instance partagée
    template<typename T>
    void registerDependency(std::shared_ptr<T> instance);

    // Enregistre une instance comme implémentation d'une interface
    template<typename TInterface, typename TImplementation>
    void registerImplementation(std::shared_ptr<TImplementation> instance);

    // Récupère une instance partagée
    template<typename T>
    std::shared_ptr<T> resolve();

    // Enregistre une fabrique pour créer l'instance à la demande
    template<typename T>
    void registerFactory(std::function<std::shared_ptr<T>()> factory);

    // Vérifie si une dépendance est enregistrée
    template<typename T>
    bool has() const;

    // Supprime une dépendance enregistrée
    template<typename T>
    bool remove();

    // Efface toutes les dépendances enregistrées
    void clear();
};
```

## ServiceLocatorAdapter

Nous avons mis en place un adaptateur qui permet une transition en douceur du `ServiceLocator` vers le `DependencyContainer`. Ce `ServiceLocatorAdapter` présente la même interface que le `ServiceLocator` existant, mais utilise en interne le `DependencyContainer`.

### Objectifs atteints

1. **Compatibilité rétroactive** : Le code existant peut continuer à fonctionner sans modification
2. **Migration progressive** : Les composants peuvent être migrés un par un vers le nouveau système
3. **Comportement identique** : Le comportement reste le même pendant la transition

### Implémentation réalisée

```cpp
class ServiceLocatorAdapter {
public:
    explicit ServiceLocatorAdapter(std::shared_ptr<DependencyContainer> container);
    ~ServiceLocatorAdapter() = default;
    
    // Initialise l'adaptateur
    void initialize(const ApplicationConfiguration& config);
    
    // Méthodes de récupération (getters)
    const ApplicationConfiguration& getConfiguration() const;
    ConfigurationService& getConfigurationService();
    // ...et d'autres getters similaires
    
    // Méthodes d'enregistrement
    void registerConfigurationService(ConfigurationService* service);
    void registerNavigationConfigService(NavigationConfigService* service);
    // ...et d'autres méthodes d'enregistrement similaires
    
private:
    std::shared_ptr<DependencyContainer> container_;
    std::shared_ptr<ApplicationConfiguration> configuration_;
};
```

### Défis surmontés

1. **Dépendances circulaires** : Nous avons dû gérer des dépendances circulaires entre certaines classes comme `ControllerService` et son écouteur d'événements. Nous avons résolu ce problème en utilisant des deleters personnalisés pour empêcher la destruction automatique :

```cpp
container_->registerDependency<ControllerService>(std::shared_ptr<ControllerService>(service, [](ControllerService*) {
    // Ne rien faire lors de la destruction
    // Pour éviter les problèmes de dépendance circulaire
}));
```

2. **Compatibilité avec les pointeurs bruts** : Le code existant utilise beaucoup de pointeurs bruts. Nous avons adapté notre système pour qu'il puisse gérer ces pointeurs correctement, tout en permettant une migration progressive vers des smart pointers.

## Tests unitaires

Nous avons mis en place des tests unitaires complets pour valider notre système d'injection de dépendances :

1. **Tests pour DependencyContainer** :
   - Enregistrement et résolution d'instances
   - Implémentations d'interfaces
   - Factories et création différée
   - Vérification de l'existence et suppression des dépendances

2. **Tests pour ServiceLocatorAdapter** :
   - Initialisation avec une configuration
   - Enregistrement et récupération de services
   - Comportement en l'absence de services enregistrés

### Adaptation aux environnements embarqués

Nous avons adapté notre approche de test pour tenir compte des contraintes des environnements embarqués :

1. **Un point d'entrée unique** : Utilisation d'un seul point d'entrée (`test_main.cpp`) pour tous les tests
2. **Mocks simplifiés** : Création de mocks qui n'ont pas de dépendances problématiques
3. **Tests ciblés** : Focus sur le comportement principal plutôt que sur les détails d'implémentation

## Prochaines étapes

Maintenant que le système d'injection de dépendances est en place, nous allons :

1. **Créer des interfaces claires** pour tous les sous-systèmes
2. **Implémenter les sous-systèmes** un par un en utilisant le nouveau système
3. **Migrer progressivement** les composants existants pour qu'ils utilisent directement le `DependencyContainer`
4. **Remplacer complètement** le `ServiceLocator` une fois que tous les composants auront été migrés

## Considérations pour l'environnement embarqué

Notre système d'injection de dépendances a été conçu en tenant compte des contraintes des microcontrôleurs Teensy :

1. **Empreinte mémoire minimale** : Utilisation judicieuse des templates pour minimiser le code généré
2. **Pas d'exceptions** : Conception qui ne repose pas sur les exceptions pour la gestion des erreurs
3. **Allocations statiques** : Minimisation des allocations dynamiques quand possible
4. **Performances** : Attention portée à l'efficacité du code pour ne pas impacter les performances en temps réel
