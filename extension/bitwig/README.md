# PC MIDI Studio - Bitwig Extension

Extension officielle pour le contrôleur **PC MIDI Studio** de PetiteChose Audio pour Bitwig Studio.

## 🎹 Caractéristiques

- Intégration complète avec Bitwig Studio
- Communication MIDI bidirectionnelle
- Retour visuel via LEDs
- Modes multiples (Mixer, Device, Transport)
- Auto-détection du contrôleur

## 📦 Installation

### Méthode rapide
1. Télécharger `PCMidiStudio.jar` depuis les [Releases](https://github.com/petitechose/pc-midi-studio/releases)
2. Copier le fichier dans:
   - **Windows**: `%USERPROFILE%\Documents\Bitwig Studio\Extensions\`
   - **macOS**: `~/Documents/Bitwig Studio/Extensions/`
   - **Linux**: `~/Documents/Bitwig Studio/Extensions/`
3. Redémarrer Bitwig Studio
4. Aller dans `Settings > Controllers` et ajouter "PC MIDI Studio"

### Compilation depuis les sources

#### Prérequis
- Java Development Kit (JDK) 11 ou supérieur
- Maven, Gradle ou Make

#### Avec Maven
```bash
mvn clean package
cp target/PCMidiStudio.jar ~/Documents/Bitwig\ Studio/Extensions/
```

#### Avec Gradle
```bash
gradle build
gradle installExtension
```

#### Avec Make
```bash
# Placer d'abord bitwig-extension-api.jar dans lib/
make all
make install
```

## 🎮 Utilisation

### Configuration initiale
1. Connecter votre PC MIDI Studio via USB
2. Dans Bitwig: `Settings > Controllers > + Add > PetiteChose Audio > PC MIDI Studio`
3. Sélectionner les ports MIDI correspondants
4. L'auto-détection devrait fonctionner automatiquement

### Modes disponibles

#### Mode Mixer (par défaut)
- **Faders**: Volume des pistes 1-8
- **Encodeurs**: Pan des pistes 1-8
- **Boutons du haut**: Mute
- **Boutons du bas**: Solo

#### Mode Device
- **Encodeurs**: Paramètres du device sélectionné
- **Boutons**: Navigation dans les devices

#### Mode Transport
- **Play/Stop/Record**: Contrôles de transport
- **Loop**: Activer/désactiver la boucle

### Raccourcis
- **Shift + Bouton**: Fonction alternative
- **Bank Left/Right**: Naviguer par groupes de 8 pistes

## 🔧 Configuration avancée

Les préférences de l'extension sont accessibles dans Bitwig:
`Settings > Controllers > PC MIDI Studio > Device Settings`

Options disponibles:
- Comportement des faders (Pickup/Jump)
- Sensibilité des encodeurs
- Couleurs des LEDs

## 🐛 Dépannage

### Le contrôleur n'est pas détecté
1. Vérifier la connexion USB
2. Vérifier que les drivers MIDI sont installés
3. Redémarrer Bitwig Studio
4. Sélectionner manuellement les ports MIDI

### Pas de retour visuel (LEDs)
1. Vérifier que le port MIDI Out est correctement configuré
2. Recharger l'extension: `File > Reload Extensions`

### Latence ou retard
1. Réduire la taille du buffer audio dans Bitwig
2. Fermer les autres applications MIDI

## 🛠️ Développement

Pour contribuer ou personnaliser l'extension, consultez [PLAN_ACTION.md](PLAN_ACTION.md) pour le guide de développement complet.

### Structure du code
```
src/main/java/audio/petitechose/controller/
├── PCMidiStudioExtensionDefinition.java  # Métadonnées
└── PCMidiStudioExtension.java           # Logique principale
```

### Debug
Activer la console de développement:
`Help > Show Control Surface Development Console`

## 📝 Changelog

### Version 1.0.0
- Support initial du PC MIDI Studio
- Modes Mixer, Device et Transport
- Auto-détection
- Retour visuel LED

## 📄 Licence

Copyright © 2024 PetiteChose Audio. Tous droits réservés.

## 📧 Support

- Email: support@petitechose.audio
- Site web: https://petitechose.audio
- Issues: https://github.com/petitechose/pc-midi-studio/issues

## 🙏 Remerciements

- L'équipe Bitwig pour leur excellente API
- La communauté Bitwig pour les retours et tests

---
*PC MIDI Studio Extension v1.0.0 - Compatible avec Bitwig Studio 5.0+*