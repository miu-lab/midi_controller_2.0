# Exemple de standardisation complète

Ce document illustre par un exemple concret l'application des standards de code.

## Exemple 1: Avant/Après pour un fichier d'interface

### Avant standardisation

```cpp
// MyInterface.hpp
#pragma once

class MyInterface {
public:
    // Destructeur virtuel
    virtual ~MyInterface() {}
    
    // Getter avec préfixe get
    virtual int getValue() = 0;
    
    // Setter sans convention claire
    virtual void updateValue(int val) = 0;
    
    // Methode dont la casse ne suit pas la convention
    virtual void Initialize() = 0;
};
```

### Après standardisation

```cpp
/**
 * @file IMyInterface.hpp
 * @brief Interface définissant les opérations sur une valeur
 */
#pragma once

/**
 * @interface IMyInterface
 * @brief Interface définissant les opérations sur une valeur
 */
class IMyInterface {
public:
    /**
     * @brief Destructeur virtuel
     */
    virtual ~IMyInterface() = default;
    
    /**
     * @brief Obtient la valeur actuelle
     * @return Valeur actuelle
     */
    virtual int value() = 0;
    
    /**
     * @brief Définit une nouvelle valeur
     * @param val Nouvelle valeur à définir
     */
    virtual void setValue(int val) = 0;
    
    /**
     * @brief Initialise l'interface
     * @return true si l'initialisation a réussi, false sinon
     */
    virtual bool init() = 0;
};
```

## Exemple 2: Avant/Après pour une classe d'implémentation

### Avant standardisation

```cpp
// ValueService.hpp
#pragma once
#include "MyInterface.hpp"

class ValueService : public MyInterface {
public:
    ValueService();
    ~ValueService();
    
    // Implémentation des méthodes
    int getValue() override;
    void updateValue(int val) override;
    void Initialize() override;
    
private:
    // Membre sans suffixe _
    int currentValue;
    bool isInitialized;
};
```

### Après standardisation

```cpp
/**
 * @file ValueService.hpp
 * @brief Service de gestion de valeur
 */
#pragma once
#include "core/domain/interfaces/IMyInterface.hpp"
#include "core/utils/Result.hpp"

/**
 * @class ValueService
 * @brief Implémentation du service de gestion de valeur
 */
class ValueService : public IMyInterface {
public:
    /**
     * @brief Constructeur par défaut
     */
    ValueService();
    
    /**
     * @brief Destructeur
     */
    ~ValueService() override = default;
    
    /**
     * @brief Obtient la valeur actuelle
     * @return Valeur actuelle
     */
    int value() override;
    
    /**
     * @brief Définit une nouvelle valeur
     * @param val Nouvelle valeur à définir
     */
    void setValue(int val) override;
    
    /**
     * @brief Initialise le service
     * @return true si l'initialisation a réussi, false sinon
     */
    bool init() override;
    
    /**
     * @brief Version améliorée de l'initialisation avec gestion d'erreurs
     * @return Résultat de l'initialisation ou message d'erreur
     */
    Result<bool, std::string> initWithErrors();
    
private:
    int currentValue_;  // Avec suffixe _
    bool initialized_;  // Avec suffixe _
};
```

## Points clés de standardisation

1. **Fichiers d'interface**: Préfixe 'I' + nom en PascalCase (ex: IMyInterface)
2. **Membres de classe**: Suffixe '_' (ex: currentValue_)
3. **Méthodes getters**: Nom sans préfixe 'get' (ex: value() au lieu de getValue())
4. **Méthodes setters**: Préfixe 'set' + nom (ex: setValue())
5. **Méthodes d'action**: Verbe en camelCase (ex: init() au lieu de Initialize())
6. **Documentation**: Commentaires Doxygen pour toutes les méthodes et classes
7. **Constructeurs et destructeurs**: Explicites et documentés
8. **Gestion d'erreurs**: Utilisation de Result<T, E> pour les opérations pouvant échouer