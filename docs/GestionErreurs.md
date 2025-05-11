# Gestion des Erreurs avec Result<T, E>

Ce document explique l'approche de gestion des erreurs basée sur `Result<T, E>` adoptée dans le projet MIDI Controller.

## Introduction

Dans les environnements embarqués comme Teensy, l'utilisation des exceptions C++ est souvent problématique:
- Augmentation significative de la taille du binaire
- Comportement imprévisible en cas de mémoire limitée
- Complexité accrue pour le débogage

Pour ces raisons, notre projet utilise un pattern `Result<T, E>` inspiré du langage Rust pour gérer les erreurs de manière explicite.

## La classe Result<T, E>

`Result<T, E>` encapsule soit une valeur de succès de type `T`, soit une erreur de type `E`:

```cpp
template <typename T, typename E>
class Result {
public:
    // Constructeurs
    explicit Result(const T& value);  // Succès
    explicit Result(const E& error);  // Erreur
    
    // Accès
    bool isSuccess() const;
    bool isError() const;
    
    const T& value() const;  // Lance une erreur si c'est un résultat d'erreur
    const E& error() const;  // Lance une erreur si c'est un résultat de succès
    
    // Chaînage optionnel
    template <typename Func>
    auto andThen(Func func) -> Result<decltype(func(std::declval<T>())), E>;
    
    // Gestion d'erreur optionnelle
    template <typename Func>
    auto orElse(Func func) -> Result<T, decltype(func(std::declval<E>()))>;
};
```

## Utilisation de Base

```cpp
// Fonction retournant un Result
Result<int, std::string> divideNumbers(int a, int b) {
    if (b == 0) {
        return Result<int, std::string>("Division par zéro");
    }
    return Result<int, std::string>(a / b);
}

// Utilisation
auto result = divideNumbers(10, 2);
if (result.isSuccess()) {
    // Utiliser result.value()
    int value = result.value();
} else {
    // Gérer l'erreur
    std::string error = result.error();
}
```

## Chaînage d'Opérations

`Result<T, E>` permet de chaîner des opérations:

```cpp
Result<int, std::string> parseAndMultiply(const std::string& input, int factor) {
    return parseInteger(input)
        .andThen([factor](int value) {
            return Result<int, std::string>(value * factor);
        });
}
```

## Propagation d'Erreurs

Le pattern permet une propagation propre des erreurs:

```cpp
Result<double, std::string> complexCalculation() {
    // Si l'une des fonctions retourne une erreur, elle sera automatiquement propagée
    auto result1 = step1();
    if (result1.isError()) return result1.error();
    
    auto result2 = step2(result1.value());
    if (result2.isError()) return result2.error();
    
    auto result3 = step3(result2.value());
    if (result3.isError()) return result3.error();
    
    // Si tout s'est bien passé, retourner le résultat final
    return Result<double, std::string>(result3.value() * 2.0);
}
```

## Dans le Contexte de notre Application

### Initialisation de Sous-systèmes

```cpp
// Dans ConfigurationSubsystem.cpp
Result<bool, std::string> ConfigurationSubsystem::init() {
    try {
        if (!loadConfiguration()) {
            return Result<bool, std::string>("Échec du chargement de la configuration");
        }
        
        // Initialisation réussie
        return Result<bool, std::string>(true);
    } catch (...) {
        return Result<bool, std::string>("Exception inattendue");
    }
}
```

### Dans MidiControllerApp

```cpp
// Dans MidiControllerApp.cpp
Result<bool, std::string> MidiControllerApp::init() {
    // Initialiser les sous-systèmes en ordre
    auto configResult = m_configSystem->init();
    if (configResult.isError()) {
        return Result<bool, std::string>("Erreur ConfigSystem: " + configResult.error());
    }
    
    auto inputResult = m_inputSystem->init();
    if (inputResult.isError()) {
        return Result<bool, std::string>("Erreur InputSystem: " + inputResult.error());
    }
    
    // ... initialisation des autres sous-systèmes
    
    return Result<bool, std::string>(true);
}
```

## Bonnes Pratiques

1. **Soyez explicite sur les types d'erreurs**
   - Utilisez des types d'erreur spécifiques plutôt que des chaînes génériques
   - Envisagez d'utiliser une classe `Error` avec un code et un message

2. **Utilisez Result pour les opérations qui peuvent échouer**
   - Les opérations I/O
   - Le parsing et la validation
   - L'initialisation de sous-systèmes

3. **Évitez de mélanger Result et les exceptions**
   - Restez cohérent dans votre approche

4. **Documentation**
   - Documentez clairement les conditions d'erreur possibles pour chaque fonction

## Limitations

- Nécessite plus de code explicite que les exceptions
- Peut rendre les signatures de fonctions plus complexes
- Nécessite une discipline constante de vérification des erreurs

## Perspectives

Pour les futures versions, nous envisageons d'enrichir notre implémentation avec:
- Fonctions utilitaires pour simplifier la propagation d'erreurs
- Support pour des codes d'erreur typés
- Intégration avec le système de logging
