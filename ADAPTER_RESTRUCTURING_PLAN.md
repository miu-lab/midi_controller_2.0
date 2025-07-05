# Plan de Restructuration des Adapters

## Structure Actuelle vs Nouvelle Structure

### Structure Actuelle (problématique)
```
src/adapters/
├── primary/ui/...          # Mélange vues et gestion événements
├── secondary/
│   ├── hardware/...        # Structure plate
│   ├── midi/...           # Pas de séparation input/output
│   └── storage/...        # OK
└── ui/lvgl/...            # Redondance avec primary/ui
```

### Nouvelle Structure Proposée (par domaine fonctionnel)
```
src/adapters/
├── hardware/               # Adapters hardware purs
│   ├── display/
│   │   ├── ILI9341Adapter.hpp
│   │   └── LvglDisplayBridge.hpp
│   └── input/
│       ├── ButtonAdapter.hpp
│       ├── EncoderAdapter.hpp
│       └── InputHardwareManager.hpp
├── communications/         # Adapters de communication
│   ├── midi/
│   │   ├── UsbMidiAdapter.hpp
│   │   ├── MidiInputAdapter.hpp
│   │   └── MidiOutputAdapter.hpp
│   └── serial/
│       └── SerialAdapter.hpp
├── storage/               # Adapters de persistance
│   ├── FlashStorageAdapter.hpp
│   ├── ProfileStorageAdapter.hpp
│   └── ConfigurationStorageAdapter.hpp
├── ui/                    # Adapters UI unifiés
│   ├── views/
│   │   ├── MenuViewAdapter.hpp
│   │   ├── ParameterViewAdapter.hpp
│   │   └── SplashViewAdapter.hpp
│   ├── components/
│   │   ├── WidgetAdapter.hpp
│   │   └── ThemeAdapter.hpp
│   └── events/
│       ├── UIEventAdapter.hpp
│       └── ViewEventListener.hpp
└── integration/           # Adapters d'intégration système
    ├── SystemBridge.hpp
    ├── PerformanceMonitor.hpp
    └── DiagnosticsAdapter.hpp
```

## Plan de Migration

### Phase 1: Consolidation UI
1. **Fusionner adapters/primary/ui et adapters/ui/lvgl**
   - Déplacer tout vers `adapters/ui/`
   - Séparer par responsabilité : views, components, events

2. **Réorganiser par type de composant**
   - Views : toutes les vues LVGL
   - Components : widgets et thèmes
   - Events : gestion des événements UI

### Phase 2: Spécialisation Hardware
1. **Séparer par type de matériel**
   - Display : tout ce qui concerne l'affichage
   - Input : boutons, encodeurs, etc.

2. **Créer des gestionnaires unifiés**
   - InputHardwareManager pour centraliser
   - DisplayManager déjà existant

### Phase 3: Communications
1. **Séparer MIDI par direction**
   - Input : réception MIDI
   - Output : envoi MIDI
   - Processing : traitement (déjà fait avec HighPerformanceMidiManager)

2. **Préparer pour extensibilité**
   - Serial communication
   - Network communication (futur)

### Phase 4: Storage et Integration
1. **Spécialiser storage par domaine**
   - Profiles
   - Configuration
   - Données utilisateur

2. **Adapters d'intégration système**
   - Monitoring de performance
   - Diagnostics
   - Bridges entre couches

## Avantages de cette Structure

### Clarté Architecturale
- **Séparation par domaine** : hardware, communications, ui, storage
- **Responsabilités claires** : chaque adapter a un rôle précis
- **Réduction de la redondance** : élimination des doublons

### Maintenabilité
- **Navigation intuitive** : facile de trouver un adapter
- **Isolation des changements** : modification d'un domaine n'affecte pas les autres
- **Tests ciblés** : tests par domaine fonctionnel

### Extensibilité
- **Ajout facilité** : nouveau hardware = nouveau fichier dans hardware/
- **Standards cohérents** : patterns similaires dans chaque domaine
- **Interfaces stables** : core/ ne change pas

## Implémentation

### Étape 1: Consolider UI (priorité haute)
```bash
# Créer nouvelle structure UI
mkdir -p src/adapters/ui/{views,components,events}

# Déplacer les fichiers
mv src/adapters/primary/ui/*.hpp src/adapters/ui/views/
mv src/adapters/ui/lvgl/widgets/* src/adapters/ui/components/
mv src/adapters/ui/lvgl/theme/* src/adapters/ui/components/
```

### Étape 2: Spécialiser Hardware
```bash
# Réorganiser hardware
mkdir -p src/adapters/hardware/{display,input}
mv src/adapters/secondary/hardware/* src/adapters/hardware/
```

### Étape 3: Communications
```bash
# Créer communications
mkdir -p src/adapters/communications/midi
mv src/adapters/secondary/midi/* src/adapters/communications/midi/
```

## Impact sur les Includes

### Avant
```cpp
#include "adapters/primary/ui/DefaultViewManager.hpp"
#include "adapters/secondary/hardware/display/Ili9341Driver.hpp"
#include "adapters/ui/lvgl/theme/UITheme.hpp"
```

### Après
```cpp
#include "adapters/ui/views/DefaultViewManager.hpp"
#include "adapters/hardware/display/Ili9341Driver.hpp"
#include "adapters/ui/components/UITheme.hpp"
```

## Compatibilité

### Migration Progressive
- **Aliases temporaires** pour maintenir compatibilité
- **Refactoring par phases** pour éviter cassure massive
- **Tests à chaque étape** pour valider fonctionnement

### Scripts de Migration
```bash
# Script pour mettre à jour les includes
find src/ -name "*.cpp" -o -name "*.hpp" | xargs sed -i 's|adapters/primary/ui/|adapters/ui/views/|g'
find src/ -name "*.cpp" -o -name "*.hpp" | xargs sed -i 's|adapters/secondary/hardware/|adapters/hardware/|g'
```

## Validation

### Critères de Succès
1. **Compilation réussie** après chaque phase
2. **Fonctionnalités préservées** : tous les tests passent
3. **Structure logique** : facile de naviguer et comprendre
4. **Performance maintenue** : pas de régression

### Points de Contrôle
- [ ] Phase 1: UI consolidée et fonctionnelle
- [ ] Phase 2: Hardware réorganisé
- [ ] Phase 3: Communications spécialisées
- [ ] Phase 4: Storage et integration finalisés

Cette restructuration améliore significativement la maintenabilité et la clarté architecturale du projet.