# Analyse des responsabilités des classes principales

## MidiControllerApp
**Responsabilités actuelles :**
- Initialisation de l'application
- Coordination des mises à jour des sous-systèmes
- Enregistrement des services dans ServiceLocator
- Gestion des configurations
- Gestion des événements

**Problèmes :**
- Trop de responsabilités
- Couplage fort avec tous les systèmes
- Gestion manuelle du cycle de vie des objets

## ServiceLocator
**Responsabilités actuelles :**
- Centralisation de l'accès aux services
- Fourniture d'un accès global aux composants
- Gestion des dépendances entre services

**Problèmes :**
- Pattern Singleton avec état global
- Difficulté à tester les composants qui l'utilisent
- Couplage fort entre tous les composants
- Dépendances implicites difficiles à suivre

## InputSystem / EventInputSystem
**Responsabilités actuelles :**
- Initialisation des périphériques d'entrée
- Lecture des états des encodeurs et boutons
- Traduction des entrées en événements
- Gestion des callbacks d'entrée

**Problèmes :**
- Duplication de code entre InputSystem et EventInputSystem
- Héritage utilisé pour la compatibilité plutôt que pour le design
- Couplage fort avec les contrôleurs UI et Input

## MidiSystem
**Responsabilités actuelles :**
- Initialisation des périphériques MIDI
- Envoi et réception de messages MIDI
- Mappage des contrôles aux messages MIDI
- Gestion des profils MIDI

**Problèmes :**
- Dépendance directe avec ProfileManager
- Mélange des préoccupations d'E/S et de logique métier
- Manque d'abstraction claire pour les tests

## UIController / ViewManager
**Responsabilités actuelles :**
- Gestion de l'affichage
- Gestion des vues et des menus
- Traitement des événements d'interface
- Navigation dans l'interface

**Problèmes :**
- Dépendance circulaire avec InputController
- Mélange des préoccupations de présentation et de logique
- Trop de responsabilités différentes

## ConfigurationService / NavigationConfigService
**Responsabilités actuelles :**
- Chargement et application des configurations
- Gestion des contrôles de navigation
- Configuration des mapages MIDI

**Problèmes :**
- Responsabilités partagées entre plusieurs services
- Manque de cohésion
- Interface peu claire

## ProfileManager
**Responsabilités actuelles :**
- Gestion des profils utilisateur
- Stockage et chargement des profils
- Fourniture des configurations aux autres systèmes

**Problèmes :**
- Couplage avec MidiSystem
- Manque d'abstraction pour le stockage

## EventBus
**Responsabilités actuelles :**
- Découplage des composants via la communication par événements
- Gestion des abonnements et des désabonnements
- Distribution des événements aux abonnés

**Problèmes :**
- Crée des dépendances implicites difficiles à suivre
- Peut masquer des problèmes de conception
- Difficile à tester de manière isolée

## Nouvelle répartition des responsabilités

### DependencyContainer
- Gestion explicite des dépendances entre composants
- Cycle de vie des objets géré par les smart pointers
- Interface claire pour l'enregistrement et la résolution des dépendances

### ConfigurationSubsystem
- Configuration centralisée de l'application
- Gestion des paramètres de navigation et des contrôles
- Interface claire pour accéder aux configurations

### InputSubsystem
- Abstraction claire des périphériques d'entrée
- Uniformisation de l'interface d'entrée
- Indépendance de l'implémentation matérielle

### MidiSubsystem
- Centralisation des opérations MIDI
- Interface simple pour l'envoi et la réception
- Séparation des préoccupations de mappage et d'E/S

### UISubsystem
- Gestion unifiée de l'interface utilisateur
- Séparation claire entre modèle et vue
- Interface adaptable pour différents types d'affichage
