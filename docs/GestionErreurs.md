# Guide de gestion des erreurs avec Result<T, E>

## Introduction

Ce document décrit le nouveau système de gestion des erreurs basé sur la classe `Result<T, E>`, qui remplace progressivement les retours de type booléen ou les pointeurs null dans le projet MIDI Controller.

## Principes

1. **Explicite plutôt qu'implicite** - Les erreurs sont représentées explicitement et non via des valeurs spéciales (comme NULL ou -1)
2. **Typage fort** - Le type des erreurs est défini clairement et permet plus de contexte
3. **Propagation contrôlée** - Les erreurs sont propagées de manière contrôlée, pas via exceptions
4. **Traitement local** - Les erreurs sont traitées au plus près de leur source

## Utilisation de Result<T, E>

### Exemple basique

```cpp
Result<int, std::string> diviser(int a, int b) {
  if (b == 0) {
    return Result<int, std::string>::error("Division par zéro");
  }
  return Result<int, std::string>::success(a / b);
}

// Utilisation
auto result = diviser(10, 2);
if (result.isSuccess()) {
  int quotient = *result.value();  // * pour accéder à la valeur de l'optional
  std::cout << "Résultat: " << quotient << std::endl;
} else {
  std::cout << "Erreur: " << *result.error() << std::endl;
}
```

### Utilisation avec match()

La méthode `match()` permet d'exécuter une lambda différente selon que le résultat est un succès ou une erreur:

```cpp
result.match(
  [](int value) { std::cout << "Succès: " << value << std::endl; },
  [](const std::string& error) { std::cout << "Erreur: " << error << std::endl; }
);
```

### Avantages par rapport aux pointeurs null

1. **Intention claire** - Le but est explicitement de représenter un résultat pouvant échouer
2. **Informations d'erreur** - Pas juste un NULL, mais une raison d'échec
3. **Sécurité de type** - Difficile d'ignorer accidentellement une erreur
4. **Compatible avec les environnements embarqués** - Ne nécessite pas d'exceptions
5. **Chaînage possible** - Possibilité de chaîner les opérations (avec des méthodes supplémentaires comme `andThen`)

## Comparaison des approches

### Avant: Utilisation de pointeurs null ou booléens

```cpp
void MidiSubsystem::sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (!initialized_ || !midiOut_) {
        return;  // Échec silencieux
    }
    
    midiOut_->sendNoteOn(MidiChannel(channel), MidiNote(note), velocity);
}
```

### Après: Utilisation de Result (sans exceptions)

```cpp
Result<bool, std::string> MidiSubsystem::sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity) {
    if (!initialized_) {
        return Result<bool, std::string>::error("MidiSubsystem: Not initialized");
    }
    
    if (!midiOut_) {
        return Result<bool, std::string>::error("MidiSubsystem: No MIDI output available");
    }
    
    // Utilisation sans exceptions, compatible avec les environnements embarqués
    midiOut_->sendNoteOn(MidiChannel(channel), MidiNote(note), velocity);
    return Result<bool, std::string>::success(true);
}
```

## Bonnes pratiques

1. **Toujours vérifier les résultats** - Ne jamais ignorer un Result sans traiter les cas d'erreur
2. **Messages d'erreur clairs** - Fournir des messages d'erreur précis et actionnables
3. **Propagation sélective** - Propager les erreurs pertinentes, traiter les autres localement
4. **Éviter les exceptions** - Utiliser Result sans blocs try/catch pour la compatibilité embarquée
5. **Immutabilité** - Un Result ne devrait pas être modifiable après création

## Méthodes disponibles

- `static success(T value)` - Crée un résultat de succès
- `static error(E error)` - Crée un résultat d'erreur
- `isSuccess()` - Vérifie si c'est un succès
- `isError()` - Vérifie si c'est une erreur
- `value()` - Récupère la valeur (std::optional)
- `error()` - Récupère l'erreur (std::optional)
- `match(onSuccess, onError)` - Exécute une lambda selon le résultat

## Migration progressive

Pour migrer progressivement vers l'utilisation de Result:

1. Identifier les méthodes retournant bool ou des pointeurs bruts
2. Convertir en `Result<T, E>` en commençant par les API critiques
3. Adapter le code appelant pour gérer explicitement les succès et échecs
4. Documenter toutes les erreurs possibles dans les commentaires Doxygen
