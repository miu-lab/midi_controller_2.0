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
