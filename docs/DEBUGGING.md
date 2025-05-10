# Guide de débogage pour le projet MIDI Controller

Ce document explique le système de débogage implémenté dans le projet MIDI Controller et comment l'utiliser efficacement.

## Configuration des environnements de débogage

Le projet propose trois configurations prédéfinies qui peuvent être sélectionnées lors de la compilation :

| Environnement | Commande | Description |
|---------------|----------|-------------|
| **Développement** | `pio run -e dev` | Configuration standard de développement avec un niveau modéré de débogage. |
| **Débogage** | `pio run -e debug` | Débogage très détaillé (verbose) pour résoudre les problèmes complexes. |
| **Débogage léger** | `pio run -e debug-light` | Débogage allégé avec les statistiques CPU basiques. |
| **Minimal** | `pio run -e minimal` | Débogage ultra-léger sans statistiques CPU ni détails. |
| **Production** | `pio run -e prod` | Version optimisée sans messages de débogage, pour le déploiement final. |

## Structure du système de débogage

Le système de débogage est centralisé dans deux fichiers principaux :

- `src/config/debug/DebugConfig.hpp` : Définit les niveaux de débogage et les configurations
- `src/config/debug/DebugMacros.hpp` : Implémente les macros de débogage

## Niveaux de débogage

Les niveaux de débogage disponibles sont :

| Niveau | Valeur | Description |
|--------|--------|-------------|
| `DEBUG_LEVEL_NONE` | 0 | Aucun message de débogage |
| `DEBUG_LEVEL_ERROR` | 1 | Erreurs seulement |
| `DEBUG_LEVEL_WARNING` | 2 | Erreurs + avertissements |
| `DEBUG_LEVEL_INFO` | 3 | Niveau informatif général (par défaut) |
| `DEBUG_LEVEL_DEBUG` | 4 | Informations détaillées pour le développement |
| `DEBUG_LEVEL_TRACE` | 5 | Suivi d'exécution très détaillé |

## Catégories de débogage

Les messages sont organisés par catégorie pour faciliter le filtrage et la lisibilité :

| Catégorie | Macro | Description |
|-----------|-------|-------------|
| Général | `DEBUG_LOG(level, format, ...)` | Message de débogage générique |
| Erreur | `DEBUG_ERROR(format, ...)` | Messages d'erreur critiques |
| Bus d'événements | `DEBUG_EVENT_BUS(format, ...)` | Événements publiés/souscrits |
| Entrées | `DEBUG_INPUT(format, ...)` | Entrées utilisateur générales |
| Encodeurs | `DEBUG_ENCODER(format, ...)` | Activité des encodeurs |
| Boutons | `DEBUG_BUTTON(format, ...)` | Activité des boutons |
| MIDI | `DEBUG_MIDI(format, ...)` | Messages MIDI |
| Interface | `DEBUG_UI(format, ...)` | Interface utilisateur |
| Ordonnanceur | `DEBUG_SCHEDULER(format, ...)` | Informations essentielles de l'ordonnanceur |
| Ordonnanceur détaillé | `DEBUG_SCHEDULER_VERBOSE(format, ...)` | Statistiques détaillées de l'ordonnanceur |

## Utilisation des macros de débogage

Pour ajouter des messages de débogage dans votre code :

```cpp
// Message d'erreur (toujours affiché en mode DEBUG)
DEBUG_ERROR("Une erreur s'est produite : %d", errorCode);

// Message d'information sur un événement
DEBUG_EVENT_BUS("Événement reçu : %s", eventName);

// Message détaillé sur l'activité d'un encodeur
DEBUG_ENCODER("Encodeur %d : valeur = %d", encoderId, value);

// Message pour le moniteur de tâches
DEBUG_SCHEDULER("Charge CPU : %.2f%%", cpuUsage);

// Utilisation de chaînes stockées en mémoire Flash
DEBUG_PRINTLN_FLASH("Initialisation terminée");
```

## Extension du système

Pour ajouter de nouvelles catégories de débogage :

1. Définir un nouveau niveau dans `DebugConfig.hpp`
2. Ajouter les macros correspondantes dans `DebugMacros.hpp`
3. Utiliser les nouvelles macros dans le code

## Macros de contrôle

Les macros suivantes permettent de contrôler le système de débogage :

| Macro | Valeurs | Description |
|-------|---------|-------------|
| `DEBUG` | défini/non défini | Active ou désactive globalement le débogage |
| `DEBUG_RAW_CONTROLS` | 0/1/2 | Niveau de détail pour les contrôles physiques |
| `DEBUG_EVENT_BUS_ENABLED` | 0/1 | Active le débogage du bus d'événements |
| `DEBUG_TASK_SCHEDULER_LEVEL` | 0/1/2 | Niveau de détail pour l'ordonnanceur (0=désactivé, 1=stats CPU basiques, 2=stats détaillées) |
| `DEBUG_LEVEL` | 0-5 | Niveau de débogage global |

## Moniteur série intelligent

Pour éviter les problèmes de saturation du moniteur série, un système de tampon intelligent a été implémenté:

- **Limitation de la longueur des lignes**: Les messages trop longs sont tronqués à 80 caractères
- **Tampon circulaire**: Seules les 300 dernières lignes sont conservées en mémoire
- **Sauts de ligne automatiques**: Chaque message est affiché sur une nouvelle ligne
- **Commandes de contrôle**: Vous pouvez interagir avec le moniteur via des commandes

### Commandes du moniteur série

Tapez ces commandes dans le moniteur série et appuyez sur Entrée:

- `clear` - Efface le moniteur et le tampon
- `dump` - Affiche tout le contenu du tampon
- `help` - Affiche l'aide des commandes disponibles

## Recommandations

- Utilisez l'environnement approprié selon le contexte
- Pour le développement quotidien, utilisez l'environnement `minimal` qui évite la saturation du moniteur série
- N'activez le débogage complet (`debug`) que lorsque nécessaire pour diagnostiquer des problèmes complexes
- Privilégiez les macros spécifiques aux catégories pour organiser les messages
- N'abusez pas des niveaux de débogage détaillés qui peuvent ralentir l'exécution
- Utilisez `DEBUG_PRINTLN_FLASH` pour les chaînes statiques afin d'économiser la mémoire RAM
- Utilisez les commandes `clear` et `dump` pour gérer efficacement le moniteur série
