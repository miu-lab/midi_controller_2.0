# Liste de vérification pour les Pull Requests

## Conventions de code
- [ ] Les noms de fichiers correspondent aux classes qu'ils contiennent
- [ ] Les membres de classe utilisent le suffixe `_`
- [ ] Les méthodes suivent la convention camelCase
- [ ] Les interfaces ont le préfixe "I"
- [ ] Le formatage respecte les standards (2 espaces d'indentation, maximum 100 caractères par ligne)
- [ ] Pas de code commenté sans raison claire

## Documentation
- [ ] Documentation Doxygen présente pour toutes les méthodes publiques
- [ ] Les paramètres sont documentés avec `@param`
- [ ] Les valeurs de retour sont documentées avec `@return`
- [ ] Les descriptions sont claires et précises
- [ ] Les erreurs potentielles sont documentées

## Gestion des erreurs
- [ ] Utilisation de `Result<T, E>` pour les opérations pouvant échouer
- [ ] Pas de pointeurs bruts (utilisation de smart pointers)
- [ ] Gestion explicite des cas d'erreur
- [ ] Messages d'erreur clairs et informatifs

## Tests
- [ ] Tests unitaires pour les nouvelles fonctionnalités
- [ ] Les tests vérifient à la fois les cas de succès et d'échec
- [ ] Les tests sont indépendants et reproductibles
- [ ] La couverture de code est satisfaisante

## Sécurité et performance
- [ ] Pas de fuites de mémoire (vérifiées avec les outils appropriés)
- [ ] Gestion appropriée des ressources (RAII)
- [ ] Pas d'allocations dynamiques inutiles
- [ ] Optimisations raisonnables pour l'environnement embarqué

## Revue fonctionnelle
- [ ] La fonctionnalité répond aux exigences
- [ ] Compatibilité avec le matériel cible (Teensy)
- [ ] Comportement correct dans des conditions limite
- [ ] Interface utilisateur intuitive (si applicable)
