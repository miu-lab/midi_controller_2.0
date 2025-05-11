# Les sous-systèmes à venir

## Introduction

Ce document présente les sous-systèmes qui seront développés dans la Phase 3 du refactoring. Ces sous-systèmes remplaceront progressivement les services existants et offriront une architecture plus modulaire, testable et maintenable.

## Vue d'ensemble de l'architecture

L'architecture révisée définit des sous-systèmes clairement séparés avec des responsabilités bien définies :

1. **ConfigurationSubsystem** : Gestion des paramètres et de la configuration de l'application
2. **InputSubsystem** : Gestion des entrées utilisateur (encodeurs, boutons)
3. **MidiSubsystem** : Gestion des communications MIDI
4. **UISubsystem** : Gestion de l'interface utilisateur et de l'affichage

Chaque sous-système implémente une interface bien définie et interagit avec les autres sous-systèmes uniquement via ces interfaces, ce qui réduit le couplage.

## Interfaces des sous-systèmes

### IConfiguration

```cpp
class IConfiguration {
public:
    virtual ~IConfiguration() = default;
    
    virtual bool isNavigationControl(ControlId id) const = 0;
    virtual void setControlForNavigation(ControlId id, bool isNavigation) = 0;
    
    // Autres méthodes de configuration...
    virtual float getEncoderSensitivity() const = 0;
    virtual void setEncoderSensitivity(float value) = 0;
    
    virtual const HardwareConfiguration& getHardwareConfiguration() const = 0;
    virtual const MappingConfiguration& getMappingConfiguration() const = 0;
};
```

### IInputSystem

```cpp
class IInputSystem {
public:
    virtual ~IInputSystem() = default;
    
    virtual void init() = 0;
    virtual void update() = 0;
    
    virtual void addButtonListener(IButtonListener* listener) = 0;
    virtual void addEncoderListener(IEncoderListener* listener) = 0;
    
    virtual void configureButton(ControlId id, const ButtonConfig& config) = 0;
    virtual void configureEncoder(ControlId id, const EncoderConfig& config) = 0;
    
    virtual ButtonState getButtonState(ControlId id) const = 0;
    virtual int getEncoderValue(ControlId id) const = 0;
};
```

### IMidiSystem

```cpp
class IMidiSystem {
public:
    virtual ~IMidiSystem() = default;
    
    virtual void init() = 0;
    virtual void update() = 0;
    
    virtual void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) = 0;
    virtual void sendNoteOff(uint8_t channel, uint8_t note) = 0;
    virtual void sendControlChange(uint8_t channel, uint8_t controller, uint8_t value) = 0;
    
    virtual void addMidiListener(IMidiListener* listener) = 0;
    virtual void setMapping(ControlId controlId, MidiControl midiControl, std::unique_ptr<MidiMappingStrategy> strategy) = 0;
    
    virtual const ProfileManager& getProfileManager() const = 0;
};
```

### IUISystem

```cpp
class IUISystem {
public:
    virtual ~IUISystem() = default;
    
    virtual void init(bool enableFullUI = false) = 0;
    virtual void update() = 0;
    
    virtual void showMessage(const std::string& message) = 0;
    virtual void clearDisplay() = 0;
    
    virtual void enterMenu() = 0;
    virtual void exitMenu() = 0;
    virtual void selectNextMenuItem() = 0;
    virtual void selectPreviousMenuItem() = 0;
    
    virtual void showMainScreen() = 0;
    virtual void showControlMonitor() = 0;
    virtual void showDebugScreen() = 0;
};
```

## Implémentation des sous-systèmes

### ConfigurationSubsystem

Ce sous-système sera responsable de gérer tous les aspects de la configuration de l'application :

- Paramètres globaux (sensibilité des encodeurs, luminosité de l'écran, etc.)
- Configuration du matériel (pins, paramètres des encodeurs et boutons)
- Mappings MIDI (assignation des contrôles aux messages MIDI)
- Configuration de la navigation (quels contrôles sont utilisés pour la navigation dans les menus)

Il implémentera l'interface `IConfiguration` et encapsulera les classes `ApplicationConfiguration`, `HardwareConfiguration` et `MappingConfiguration` existantes.

### InputSubsystem

Ce sous-système gérera toutes les entrées utilisateur :

- Lecture des encodeurs et boutons
- Génération d'événements pour les changements d'état
- Application des configurations (sensibilité, debouncing, etc.)
- Filtrage et traitement des entrées

Il implémentera l'interface `IInputSystem` et encapsulera les classes `EncoderManager`, `DigitalButtonManager` et `EventInputSystem` existantes.

### MidiSubsystem

Ce sous-système sera responsable de toutes les communications MIDI :

- Envoi et réception de messages MIDI
- Application des stratégies de mapping (absolu, relatif, etc.)
- Gestion des profils MIDI
- Gestion du buffer MIDI pour éviter les pertes de messages

Il implémentera l'interface `IMidiSystem` et encapsulera les classes `MidiSystem`, `MidiMapper` et `BufferedMidiOut` existantes.

### UISubsystem

Ce sous-système gérera l'interface utilisateur :

- Affichage sur l'écran OLED
- Gestion des différentes vues (menu, moniteur, débug)
- Navigation dans les menus
- Affichage des messages et alertes

Il implémentera l'interface `IUISystem` et encapsulera les classes `ViewManager`, `MenuView`, `ControlMonitorView`, etc. existantes.

## Plan d'implémentation

### Étape 1 : Interfaces et ConfigurationSubsystem

1. Définir toutes les interfaces dans des fichiers séparés
2. Implémenter `ConfigurationSubsystem` qui est le moins dépendant des autres
3. Tester l'implémentation avec des tests unitaires

### Étape 2 : InputSubsystem

1. Adapter `EventInputSystem` en `InputSubsystem`
2. Implémenter l'interface `IInputSystem`
3. Tester avec les encodeurs et boutons

### Étape 3 : MidiSubsystem

1. Adapter `MidiSystem` en `MidiSubsystem`
2. Implémenter l'interface `IMidiSystem`
3. Tester avec des messages MIDI

### Étape 4 : UISubsystem

1. Créer une nouvelle implémentation de `IUISystem`
2. Adapter le code existant pour utiliser cette nouvelle implémentation
3. Tester avec différentes vues et actions utilisateur

## Considérations pour les tests

Chaque sous-système sera accompagné de tests unitaires et d'intégration. Les tests unitaires valideront le comportement isolé de chaque sous-système, tandis que les tests d'intégration vérifieront les interactions entre sous-systèmes.

Pour faciliter les tests, nous utiliserons des mocks pour simuler les dépendances externes :

- `MockInput` pour simuler les entrées utilisateur
- `MockMidi` pour simuler les communications MIDI
- `MockUI` pour simuler l'interface utilisateur

## Conclusion

Cette nouvelle architecture par sous-systèmes permettra une meilleure séparation des préoccupations, une plus grande testabilité et une maintenance facilitée. Elle servira de fondation solide pour les futures évolutions du contrôleur MIDI.
