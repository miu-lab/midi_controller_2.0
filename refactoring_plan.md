# Plan de Refactorisation - Configuration Unifiée

## Vue d'ensemble

Ce document détaille le plan de refactorisation pour unifier les configurations Hardware et Mapping du contrôleur MIDI. L'objectif est d'éliminer la duplication, améliorer la maintenabilité et garantir la cohérence entre les couches.

## Architecture Actuelle vs Cible

### 🔴 Architecture Actuelle (Problèmes)
- **Duplication** : IDs répétés dans HardwareConfiguration et MappingConfiguration
- **Couplage implicite** : Relation basée uniquement sur les IDs
- **Verbosité** : ~300 lignes pour configurer 10 contrôles
- **Risque d'erreurs** : Possibilité de mappings orphelins
- **Convention non documentée** : ID boutons = 1000 + ID encodeur

### 🟢 Architecture Cible (Solutions)
- **Source unique** : ControlDefinition unifie hardware + mappings
- **API Builder** : Configuration fluide et expressive
- **Validation** : Impossible d'avoir des incohérences
- **Concision** : ~50 lignes pour la même configuration
- **Extensibilité** : Support JSON/YAML, hot-reload, présets

## Phase 1 : Préparation et Tests

### 1.1 Analyse de l'existant
- [ ] Inventorier tous les usages de HardwareConfiguration
- [ ] Inventorier tous les usages de MappingConfiguration
- [ ] Identifier les dépendances externes (InputManager, MidiProcessor, etc.)
- [ ] Documenter les conventions implicites (ID 1000+, groupes, etc.)
- [ ] Créer un diagramme de dépendances

### 1.2 Mise en place des tests
- [ ] Créer des tests unitaires pour HardwareConfiguration actuelle
- [ ] Créer des tests unitaires pour MappingConfiguration actuelle
- [ ] Créer des tests d'intégration pour le flux complet
- [ ] Établir une baseline de performance
- [ ] Documenter le comportement attendu

### 1.3 Création de l'infrastructure de test
- [ ] Créer un mock de InputManager
- [ ] Créer un mock de MidiProcessor
- [ ] Créer des fixtures de configuration
- [ ] Mettre en place des tests de non-régression
- [ ] Configurer CI/CD pour les tests

## Phase 2 : Implémentation de la Configuration Unifiée

### 2.1 Structures de base
- [ ] Créer le dossier `src/config/unified/`
- [ ] Implémenter `ControlDefinition.hpp`
  - [ ] Structure de base
  - [ ] Méthodes utilitaires (hasRole, getMappingsForRole, etc.)
  - [ ] Tests unitaires
- [ ] Implémenter `MappingRole` enum si nécessaire
- [ ] Implémenter `MappingControlType` enum si nécessaire

### 2.2 Builder Pattern
- [ ] Implémenter `ControlBuilder.hpp`
  - [ ] Méthodes de configuration hardware
  - [ ] Méthodes de configuration mapping
  - [ ] Helpers pour cas courants (asMidiEncoder)
  - [ ] Validation dans build()
  - [ ] Tests unitaires complets
- [ ] Créer des tests pour tous les scénarios
  - [ ] Encodeur MIDI simple
  - [ ] Encodeur avec bouton
  - [ ] Bouton standalone
  - [ ] Contrôles multi-rôles

### 2.3 Configuration Unifiée
- [ ] Implémenter `UnifiedConfiguration.hpp`
- [ ] Implémenter `UnifiedConfiguration.cpp`
  - [ ] addControl()
  - [ ] findControlById()
  - [ ] getControlsByRole()
  - [ ] validate()
  - [ ] getStats()
- [ ] Tests unitaires pour chaque méthode
- [ ] Tests de validation des contraintes

### 2.4 Générateurs de compatibilité
- [ ] Implémenter generateHardwareConfig()
  - [ ] Conversion ControlDefinition → InputConfig
  - [ ] Gestion des boutons d'encodeurs
  - [ ] Préservation de l'ordre et des groupes
  - [ ] Tests de conversion
- [ ] Implémenter generateMappingConfig()
  - [ ] Conversion mappings → InputMapping
  - [ ] Génération de l'index
  - [ ] Gestion des NavigationControl
  - [ ] Tests de conversion
- [ ] Tests d'intégration avec les classes existantes

### 2.5 Factory de configuration
- [ ] Créer `ConfigurationFactory.cpp`
  - [ ] Méthode createDefaultConfiguration()
  - [ ] Helpers pour les pins
  - [ ] Configuration complète du système
- [ ] Valider que la sortie est identique à l'actuelle
- [ ] Tests de non-régression

## Phase 3 : Migration Progressive

### 3.1 Adapter les points d'entrée
- [ ] Modifier main.cpp pour utiliser UnifiedConfiguration
  ```cpp
  auto unified = ConfigurationFactory::createDefaultConfiguration();
  auto hwConfig = unified->generateHardwareConfig();
  auto mapConfig = unified->generateMappingConfig();
  ```
- [ ] Tester sur hardware réel
- [ ] Valider le comportement MIDI
- [ ] Valider la navigation
- [ ] Mesurer les performances

### 3.2 Migration des consommateurs (optionnel)
- [ ] Identifier les classes qui pourraient bénéficier de l'API unifiée
- [ ] Créer des adapters si nécessaire
- [ ] Migrer progressivement chaque consommateur
- [ ] Maintenir la rétrocompatibilité

### 3.3 Nettoyage progressif
- [ ] Marquer HardwareConfiguration comme deprecated
- [ ] Marquer MappingConfiguration comme deprecated
- [ ] Documenter la nouvelle API
- [ ] Créer un guide de migration

## Phase 4 : Fonctionnalités Avancées

### 4.1 Sérialisation JSON
- [ ] Définir le schéma JSON
  ```json
  {
    "controls": [{
      "id": 71,
      "name": "encoder_1",
      "hardware": {...},
      "mappings": [...]
    }]
  }
  ```
- [ ] Implémenter to_json(ControlDefinition)
- [ ] Implémenter from_json(ControlDefinition)
- [ ] Implémenter load/save dans UnifiedConfiguration
- [ ] Tests de sérialisation/désérialisation
- [ ] Validation du schéma

### 4.2 Hot-reload
- [ ] Ajouter un FileWatcher
- [ ] Implémenter reloadConfiguration()
- [ ] Gérer les callbacks de changement
- [ ] Tester la stabilité lors du rechargement
- [ ] Documenter les limitations

### 4.3 Système de présets
- [ ] Définir la structure des présets
- [ ] Créer des présets standards
  - [ ] "Default MIDI Controller"
  - [ ] "DJ Controller"
  - [ ] "DAW Controller"
- [ ] Système de merge de présets
- [ ] UI pour sélectionner les présets

### 4.4 Validation avancée
- [ ] Détection de conflits GPIO
- [ ] Détection de doublons d'ID
- [ ] Validation des plages MIDI
- [ ] Suggestions de correction
- [ ] Rapport de validation détaillé

## Phase 5 : Finalisation

### 5.1 Documentation
- [ ] Documenter l'architecture (diagrammes UML)
- [ ] Guide d'utilisation du ControlBuilder
- [ ] Exemples de configurations
- [ ] Guide de migration depuis l'ancienne API
- [ ] FAQ et troubleshooting

### 5.2 Tests finaux
- [ ] Tests de performance comparative
- [ ] Tests de consommation mémoire
- [ ] Tests de stabilité longue durée
- [ ] Tests sur différentes configurations
- [ ] Validation par les utilisateurs

### 5.3 Déploiement
- [ ] Créer une branche de release
- [ ] Code review complet
- [ ] Merger dans develop
- [ ] Tests d'intégration finaux
- [ ] Merger dans main
- [ ] Tag de version

### 5.4 Post-déploiement
- [ ] Monitorer les issues
- [ ] Collecter les retours utilisateurs
- [ ] Planifier les améliorations futures
- [ ] Retirer le code deprecated (après période de transition)

## Métriques de Succès

### Quantitatives
- ✅ Réduction de 70% du code de configuration
- ✅ 0 duplication d'ID
- ✅ 100% de couverture de tests
- ✅ Performance identique ou meilleure
- ✅ Consommation mémoire réduite

### Qualitatives
- ✅ Configuration plus intuitive
- ✅ Moins d'erreurs de configuration
- ✅ Maintenance simplifiée
- ✅ Extension facilitée
- ✅ Documentation claire

## Risques et Mitigation

| Risque | Impact | Probabilité | Mitigation |
|--------|--------|-------------|------------|
| Régression fonctionnelle | Élevé | Moyen | Tests exhaustifs, migration progressive |
| Performance dégradée | Moyen | Faible | Benchmarks, profiling |
| Résistance au changement | Moyen | Moyen | Documentation, période de transition |
| Bugs dans la conversion | Élevé | Moyen | Tests unitaires, validation |

## Timeline Estimée

- **Phase 1** : 2-3 jours (Préparation et tests)
- **Phase 2** : 5-7 jours (Implémentation core)
- **Phase 3** : 3-4 jours (Migration)
- **Phase 4** : 5-7 jours (Fonctionnalités avancées) - Optionnel
- **Phase 5** : 2-3 jours (Finalisation)

**Total** : 17-24 jours (12-14 jours pour le MVP sans Phase 4)

## Décisions à Prendre

1. **Portée du MVP** : Inclure Phase 4 ou la reporter ?
2. **Rétrocompatibilité** : Durée de la période de transition ?
3. **Format de sérialisation** : JSON, YAML, ou les deux ?
4. **Stratégie de migration** : Big bang ou progressive ?
5. **Gestion des présets** : Intégrés ou plugins ?

## Prochaines Étapes

1. Valider ce plan avec l'équipe
2. Créer les issues GitHub pour chaque tâche
3. Estimer plus précisément chaque sous-tâche
4. Définir les priorités
5. Commencer par Phase 1.1 : Analyse de l'existant