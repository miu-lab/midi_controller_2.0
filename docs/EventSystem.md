# Système d'Événements

## Présentation

Le Système d'Événements est un composant central de l'application qui facilite la communication entre les différentes parties du programme. Il implémente un modèle de publication/abonnement (pub/sub) optimisé pour les environnements embarqués.

## Caractéristiques principales

- **Allocation statique** : Utilise des tableaux à taille fixe au lieu d'allocations dynamiques
- **Performances optimales** : Réduit les copies et les allocations pour une latence minimale
- **Gestion de priorités** : Les écouteurs peuvent être enregistrés avec différentes priorités
- **Flux d'événements typés** : Types d'événements spécifiques et fortement typés

# Organisation du code

Le système d'événements est organisé dans le répertoire `src/core/domain/events` :

- `core/` : Contient les composants principaux du système
  - `Event.hpp` : Classe de base pour tous les événements
  - `EventBus.hpp` : Bus d'événements centralisé
  - `EventTypes.hpp` : Types d'événements spécifiques
  - `EventDispatcher.hpp` : Outil pour dispatcher des événements
  - `Events.hpp` : Fichier d'inclusion combinant les composants ci-dessus

- `EventSystem.hpp` : Point d'entrée principal du système (inclut `core/Events.hpp`)

## Utilisation

### Fichier d'inclusion principal

```cpp
#include "core/domain/events/EventSystem.hpp"
```

### Publier un événement

```cpp
// Créer un événement
EncoderTurnedEvent event(encoderId, position, delta);

// Publier l'événement sur le bus
EventBus::getInstance().publish(event);
```

### S'abonner aux événements

```cpp
// Créer une classe d'écouteur
class MyEventListener : public EventListener {
public:
    virtual bool onEvent(const Event& event) override {
        // Traiter l'événement
        switch (event.getType()) {
            case EventTypes::EncoderTurned:
                // Traiter l'événement EncoderTurned
                return true;
            // ...
        }
        return false;
    }
};

// S'abonner au bus d'événements
MyEventListener* listener = new MyEventListener();
SubscriptionId id = EventBus::getInstance().subscribe(listener);
```

### Se désabonner

```cpp
EventBus::getInstance().unsubscribe(id);
```

## Types d'événements disponibles

- **Encodeurs**
  - `EncoderTurnedEvent` : Rotation d'un encodeur
  - `EncoderButtonEvent` : Appui sur un bouton d'encodeur

- **Boutons**
  - `ButtonPressedEvent` : Appui sur un bouton
  - `ButtonReleasedEvent` : Relâchement d'un bouton

- **Interface utilisateur**
  - `ScreenChangeEvent` : Changement d'écran
  - `MenuNavigationEvent` : Navigation dans un menu
  - `DialogShowEvent` : Affichage d'une boîte de dialogue

- **MIDI**
  - `MidiNoteOnEvent` : Note MIDI activée
  - `MidiNoteOffEvent` : Note MIDI désactivée
  - `MidiControlChangeEvent` : Contrôleur MIDI modifié

## Exemples d'écouteurs

- `UIEventListener` : Écoute les événements d'interface utilisateur
- `UIControllerEventListener` : Écoute les événements de navigation
- `MidiSystemEventListener` : Écoute les événements MIDI
