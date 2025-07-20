# MIDI Controller - Bibliothèque de Composants Unifiée

Cette bibliothèque contient tous les composants nécessaires pour le projet de contrôleur MIDI, organisés de manière cohérente et professionnelle.

## Structure de la Bibliothèque

```
midi_controller/
├── midi_controller.kicad_sym     # Symboles schématiques unifiés
├── midi_controller.pretty/       # Footprints PCB
├── 3d_models/                   # Modèles 3D
├── datasheets/                  # Documentation technique
└── README.md                    # Ce fichier
```

## Composants Disponibles

### 🎛️ Microcontrôleurs
- **MIDI_Teensy4.1** - Teensy 4.1 (ARM Cortex-M7 600MHz)
  - Footprint: `Teensy41.kicad_mod`
  - Modèle 3D: `Teensy_4.1_Assembly.STEP`
  - Datasheet: https://www.pjrc.com/store/teensy41.html

### 🖥️ Affichages
- **MIDI_Display_MSP2807** - Écran TFT 2.8" ILI9341 320x240 avec touch
  - Footprint: `MSP2807_ILI9341_Display`
  - Interface: SPI (display) + SPI (touch)
  - Alimentation: 3.3V/5V
  - Résolution: 320×240 pixels
  - Contrôleur: ILI9341
  - Touch controller intégré
  - Utilisé avec LVGL 9.3.0

### 🔄 Encodeurs
- **MIDI_Encoder_PEC11R** - Encodeur mécanique Bourns PEC11R
  - Footprint: `PEC11R4220FS0024.kicad_mod`
  - Modèle 3D: `PEC11R-4020F-S0024.stp`
  - Résolution: 24 impulsions/tour
  - Avec bouton poussoir intégré

- **MIDI_Encoder_E38S6G5_Optical** - Encodeur optique haute précision
  - Footprint: `E38S6G5_Optical_Encoder`
  - Modèle: E38S6G5-600B-G24N
  - Résolution: 600 pulses/révolution
  - Diamètre: Ø38mm
  - Sortie: Open Collector NPN (phases A, B)
  - Alimentation: 5-24V
  - Montage: 3 vis M4 sur cercle Ø30mm
  - Type: Optique incrémental haute précision

### 🔘 Boutons
- **MIDI_Button_Tactile** - Bouton tactile momentané
  - Footprint: `Button_Tactile_6x6mm` (à créer)
  - Type: Momentané NO (Normalement Ouvert)

## Convention de Nommage

### Symboles
- Préfixe: `MIDI_`
- Format: `MIDI_[Catégorie]_[Modèle]`
- Exemple: `MIDI_Encoder_PEC11R`

### Footprints
- Nom descriptif sans préfixe
- Format: `[Modèle]_[Spécification]`
- Exemple: `PEC11R4220FS0024`

### Modèles 3D
- Nom du modèle + extension
- Formats supportés: `.step`, `.stp`, `.wrl`

## Utilisation dans KiCad

### 1. Intégration Automatique
La bibliothèque est automatiquement intégrée via `sym-lib-table` :
```
(lib (name "midi_controller")(type "KiCad")(uri "${KIPRJMOD}/component_library/midi_controller/midi_controller.kicad_sym")(options "")(descr "Unified MIDI Controller Component Library"))
```

### 2. Accès aux Composants
1. Ouvrir l'éditeur de schématique
2. Placer un symbole (touche 'A')
3. Chercher dans la bibliothèque `midi_controller`
4. Sélectionner le composant désiré

### 3. Footprints
Les footprints sont automatiquement associés aux symboles via la propriété `Footprint` :
- Format: `midi_controller:[NomFootprint]`
- Exemple: `midi_controller:Teensy41`

## Gestion des Versions

### Structure de Version
- **v1.0** : Version initiale avec composants de base
- **v1.x** : Ajouts de composants, corrections mineures
- **v2.x** : Modifications majeures de structure

### Changelog
- **v1.0** (2025-01-20) : Création de la bibliothèque unifiée
  - Migration depuis bibliothèques séparées
  - Standardisation des noms et propriétés
  - Documentation complète

## Ajout de Nouveaux Composants

### Processus
1. **Créer le symbole** dans `midi_controller.kicad_sym`
2. **Ajouter le footprint** dans `midi_controller.pretty/`
3. **Inclure modèle 3D** dans `3d_models/` si disponible
4. **Ajouter documentation** dans `datasheets/` si nécessaire
5. **Mettre à jour** ce README.md

### Standards Requis
- **Documentation Doxygen** pour toutes les propriétés
- **Mots-clés** appropriés (`ki_keywords`)
- **Footprint associé** (`Footprint` property)
- **Datasheet URL** quand disponible
- **Description claire** du composant

## Maintenance

### Vérifications Régulières
- [ ] Cohérence des noms de symboles/footprints
- [ ] Validité des liens de datasheets
- [ ] Présence des modèles 3D
- [ ] Conformité aux standards KiCad

### Backup
- La bibliothèque est versionnée avec le projet
- Sauvegardes automatiques dans `midi_controller-backups/`

## Support

Pour toute question ou amélioration :
1. Créer une issue dans le projet
2. Proposer des modifications via pull request
3. Consulter la documentation KiCad officielle

---
*Bibliothèque maintenue pour le projet MIDI Controller v2.0*