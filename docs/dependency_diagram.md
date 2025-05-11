# Diagramme des dépendances actuelles

```
+---------------+     dépend de     +------------------+
| main.cpp      |------------------>| MidiControllerApp |
+---------------+                   +------------------+
                                         |
                                         | initialise & utilise
                                         v
+---------------+                   +------------------+
| AppConfig     |<------------------|                  |
+---------------+                   |                  |
                                    |                  |
+---------------+                   |                  |
| EventBus      |<----------------->|                  |
+---------------+                   |                  |
                                    |  ServiceLocator  |
+---------------+                   |                  |
| InputSystem   |<----------------->|                  |
+---------------+                   |                  |
                                    |                  |
+---------------+                   |                  |
| MidiSystem    |<----------------->|                  |
+---------------+                   +------------------+
      ^                                    ^
      |                                    |
      +------------------------------------+
                dépendance mutuelle

+--------------------+                +------------------+
| UIController       |<-------------->| InputController  |
+--------------------+                +------------------+
          ^                                 ^
          |                                 |
          |                                 |
+---------+-----------+          +---------+------------+
| ViewManager         |          | EventInputSystem     |
+---------------------+          +----------------------+
                                          |
                                          | hérite de
                                          v
                                 +----------------------+
                                 | InputSystem          |
                                 +----------------------+
```

## Légende

- Les flèches indiquent les dépendances directes (une classe utilise l'autre)
- Les flèches bidirectionnelles indiquent des dépendances mutuelles (cycles)
- Les composants sont regroupés par fonction

## Problèmes identifiés

1. **Dépendances centralisées** : Presque tous les composants dépendent de ServiceLocator
2. **Cycles de dépendances** : Notamment entre UIController et InputController
3. **Héritage problématique** : EventInputSystem hérite de InputSystem mais réimplémente ses fonctionnalités
4. **Responsabilités multiples** : MidiControllerApp a trop de responsabilités différentes

## Structure visée après refactoring

```
                       +------------------+
                       | MidiControllerApp2 |
                       +------------------+
                               |
                               | utilise
                               v
                       +------------------+
                       | DependencyContainer |
                       +------------------+
                             / | \
                            /  |  \
                           /   |   \
          +---------------+    |    +---------------+    +---------------+
          | InputSubsystem |    |    | MidiSubsystem |    | UISubsystem   |
          +---------------+    |    +---------------+    +---------------+
                 ^             |            ^                   ^
                 |             v            |                   |
                 |    +----------------+    |                   |
                 +--->| ConfigSubsystem |<--+-------------------+
                      +----------------+
```

Dans cette nouvelle structure :
- Les sous-systèmes communiquent via des interfaces bien définies
- DependencyContainer gère explicitement les dépendances
- Les cycles de dépendances sont éliminés
- Chaque composant a une responsabilité unique et claire
