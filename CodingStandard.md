# MIDI Controller - Standards de Code

## Conventions de nommage

### Variables et membres
- Membres de classe : suffixe underscore (`_`) 
  - Exemple : `config_`, `display_`, `midiSystem_`
- Variables locales : camelCase sans suffixe 
  - Exemple : `localVariable`, `encoderConfig`, `buttonState`
- Constantes et macros : SNAKE_CASE majuscules 
  - Exemple : `MAX_ENCODERS`, `DEFAULT_CHANNEL`
- Énumérations : PascalCase pour le type, PascalCase pour les valeurs
  - Exemple : `enum class ButtonState { Pressed, Released, Held }`

### Classes et méthodes
- Noms de classes : PascalCase 
  - Exemple : `ConfigurationSubsystem`, `MidiMapper`
- Interfaces : préfixe "I" + PascalCase 
  - Exemple : `IInputSystem`, `IMidiOut`
- Noms de méthodes : camelCase 
  - Exemple : `sendMidiMessage()`, `updateDisplay()`
- Méthodes getter : directement le nom 
  - Exemple : `encoderSpeed()` (non `getEncoderSpeed()`)
- Méthodes setter : préfixe "set" 
  - Exemple : `setEncoderSpeed()`

### Fichiers
- Noms de fichiers identiques à la classe qu'ils contiennent
- Extension `.hpp` pour les headers, `.cpp` pour l'implémentation

## Formatage
- Indentation : 2 espaces
- Accolades : style K&R (accolade ouvrante sur la même ligne)
- Longueur de ligne maximale : 100 caractères

## Documentation
- Commentaires Doxygen pour toute l'API publique
- Tous les paramètres doivent être documentés avec `@param`
- Valeurs de retour documentées avec `@return`
- Description brève au début de chaque classe et méthode

## Gestion des erreurs
- Utiliser `Result<T, E>` pour les opérations pouvant échouer
- Éviter les pointeurs bruts, préférer les smart pointers
- Documenter les cas d'erreur possibles
- Éviter les exceptions (coûteuses en ressources et imprévisibles en embarqué)
- Toujours valider les entrées et vérifier les limites des opérations

## Gestion de la mémoire
- Préférer l'allocation statique à l'allocation dynamique
- Utiliser des structures de données à taille fixe (tableaux statiques plutôt que `std::vector`)
- Éviter la fragmentation de la mémoire
- Passer les objets volumineux par référence constante (`const Type&`) plutôt que par valeur
- Utiliser des buffers circulaires pour les opérations de streaming (comme les messages MIDI)

## Optimisation du code
- Éviter la récursion qui peut conduire à des débordements de pile
- Privilégier les opérations bit à bit quand c'est possible
- Limiter les opérations coûteuses (division, modulo, virgule flottante)
- Utiliser le type de données le plus petit adapté à l'usage (`uint8_t` vs `int`)
- Utiliser `constexpr` pour le calcul à la compilation quand c'est possible
- Définir des constantes nommées plutôt que des "magic numbers"

## Structure et architecture
- Adopter une architecture modulaire
- Suivre le principe de responsabilité unique
- Utiliser les interfaces pour découpler l'implémentation
- Appliquer l'injection de dépendances pour faciliter les tests
- Garder les fonctions courtes, particulièrement dans le code critique ou les interruptions

## Timing et temps réel
- Éviter les délais bloquants (`delay()`)
- Utiliser des machines à états ou des timers non-bloquants
- Planifier les tâches pour maintenir une bonne réactivité du système
- Mesurer et optimiser les performances des fonctions critiques
- Pour MIDI, assurer une précision de timing en évitant les opérations introduisant de la jitter

## Spécificités MIDI
- Traiter les messages MIDI par lots quand c'est possible
- Implémenter des buffers circulaires pour les messages MIDI
- Respecter les spécifications MIDI pour le timing
- Gérer efficacement les canaux MIDI multiples

## Tests
- Implémenter des tests unitaires même en environnement embarqué
- Utiliser des techniques de mocking pour simuler le matériel dans les tests
- Effectuer des tests d'intégration pour les différents composants
- Vérifier les cas limites et les conditions d'erreur

## Bonnes pratiques de version et collaboration
- Commits atomiques et descriptifs
- Branches fonctionnelles
- Pull requests pour revue de code
- Intégration continue quand c'est possible
