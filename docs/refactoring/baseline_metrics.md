# Métriques de Base - Configuration Actuelle

## Métriques de Code

### Taille des Fichiers
- `HardwareConfiguration.cpp` : ~280 lignes
- `HardwareConfiguration.hpp` : ~45 lignes  
- `MappingConfiguration.cpp` : ~220 lignes
- `MappingConfiguration.hpp` : ~40 lignes
- **Total** : ~585 lignes

### Duplication de Code
- **Pattern Encodeur** : Répété 10 fois (8 MIDI + 1 nav + 1 optique)
- **Pattern Bouton Encodeur** : Répété 9 fois
- **Pattern Mapping MIDI** : Répété 17 fois
- **Lignes dupliquées estimées** : ~400 lignes (68%)

### Complexité
- **Nombre de fichiers à modifier pour ajouter un encodeur** : 2
- **Nombre de lignes à ajouter** : ~25-30
- **Points de synchronisation** : 3 (ID hardware, ID bouton +1000, mapping)

## Métriques de Configuration

### Inventaire des Contrôles
| Type | Quantité | IDs |
|------|----------|-----|
| Encodeurs MIDI | 8 | 71-78 |
| Encodeur Navigation | 1 | 79 |
| Encodeur Optique | 1 | 80 |
| Boutons Standalone | 2 | 51-52 |
| Boutons Encodeurs | 9 | 1071-1079 |
| **Total** | **21** | - |

### Mappings MIDI
| Contrôle | Type | CC/Note |
|----------|------|---------|
| Encodeurs 71-78 | CC | 1-8 |
| Boutons 1071-1078 | CC | 36-43 |
| Encodeur 80 | CC | 10 |

### Contrôles Navigation
| ID | Type | Action |
|----|------|--------|
| 51 | Bouton | HOME |
| 52 | Bouton | BACK |
| 79 | Encodeur | ITEM_NAVIGATOR |
| 1079 | Bouton | ITEM_VALIDATE |

## Métriques de Performance (Baseline)

### Temps de Recherche (1000 itérations)
- `getInputConfigurationById()` : ~15ms
- `getMidiMappingForControl()` : ~8ms
- **Overhead de double configuration** : +100%

### Utilisation Mémoire (Estimée)
- `HardwareConfiguration` : ~2KB
- `MappingConfiguration` : ~1.5KB
- **Total Configuration** : ~3.5KB
- **Duplication de données** : ~30%

## Conventions Non Documentées

1. **Convention ID 1000+**
   - Règle : `ButtonId = EncoderId + 1000`
   - Appliquée : 9 fois
   - Documentation : Aucune

2. **Convention Display Order**
   - Boutons : `displayOrder = id - 50`
   - Encodeurs : `displayOrder = id - 70`
   - Documentation : Commentaire inline uniquement

3. **Convention Groupes**
   - "MIDI" : Encodeurs de contrôle
   - "Navigation" : Contrôles UI
   - "Precision" : Encodeur haute résolution
   - Documentation : Implicite

## Risques Identifiés

### Risques Élevés
1. **Désynchronisation ID** : Oubli de la convention 1000+
2. **Mapping Orphelin** : Mapping sans hardware correspondant
3. **Duplication Pin** : Même pin utilisé deux fois

### Risques Moyens
1. **Incohérence Type** : ENCODER mappé comme BUTTON
2. **Groupe Incorrect** : Navigation dans groupe MIDI
3. **CC Dupliqué** : Même CC pour plusieurs contrôles

### Risques Faibles
1. **Display Order** : Ordre d'affichage incorrect
2. **Performance** : Recherche linéaire dans les vecteurs
3. **Mémoire** : Duplication des métadonnées

## Objectifs de la Refactorisation

### Quantitatifs
- ✅ Réduire le code de 70% (585 → ~175 lignes)
- ✅ Éliminer 100% de la duplication
- ✅ Réduire à 1 fichier à modifier pour ajouter un contrôle
- ✅ 0 point de synchronisation manuelle

### Qualitatifs
- ✅ Documentation automatique des conventions
- ✅ Validation impossible à contourner
- ✅ API intuitive et expressive
- ✅ Migration transparente

## Critères de Succès

1. **Tous les tests passent** (0 régression)
2. **Performance identique ou meilleure**
3. **Mémoire réduite d'au moins 20%**
4. **Code réduit d'au moins 60%**
5. **100% de compatibilité** avec le système existant

## Prochaines Mesures

Après implémentation de UnifiedConfiguration :
- [ ] Mesurer la nouvelle taille de code
- [ ] Comparer les performances
- [ ] Vérifier l'utilisation mémoire
- [ ] Valider la facilité d'ajout de contrôles
- [ ] Documenter les améliorations
