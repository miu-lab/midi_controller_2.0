#!/bin/bash
# check_method_names.sh - Script pour vérifier la standardisation des noms de méthodes

# Déterminer le chemin de base du projet
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Vérifier si le répertoire source existe
if [ ! -d "$PROJECT_ROOT/src" ]; then
    echo "ERREUR: Le répertoire src n'existe pas au chemin $PROJECT_ROOT/src"
    echo "Le script doit être exécuté depuis la racine du projet ou le répertoire tools"
    exit 1
fi

# Dossiers à analyser
SRC_DIR="$PROJECT_ROOT/src"
INTERFACES_DIR="$SRC_DIR/core/domain/interfaces"
SUBSYSTEMS_DIR="$SRC_DIR/app/subsystems"
DI_DIR="$SRC_DIR/app/di"

# Créer dossier de résultats
mkdir -p "$PROJECT_ROOT/analysis_results"

RESULT_FILE="$PROJECT_ROOT/analysis_results/method_names.txt"
echo "=== Analyse des noms de méthodes ===" > "$RESULT_FILE"

# 1. Vérifier les getters qui ne suivent pas les conventions (get[A-Z])
echo "=== Getters avec préfixe 'get' ===" >> "$RESULT_FILE"
grep -r "get[A-Z]" --include="*.hpp" $INTERFACES_DIR $SUBSYSTEMS_DIR | head -20 >> "$RESULT_FILE"

# 2. Vérifier les setters qui ne suivent pas le format set[A-Z]
echo -e "\n=== Méthodes de type setter non conformes ===" >> "$RESULT_FILE"
grep -r "void " --include="*.hpp" $INTERFACES_DIR $SUBSYSTEMS_DIR | grep -v "set[A-Z]" | grep -E "modify|update|change|configure" | head -20 >> "$RESULT_FILE"

# 3. Vérifier les méthodes avec mauvaise casse (commence par une majuscule)
echo -e "\n=== Méthodes commençant par une majuscule ===" >> "$RESULT_FILE"
grep -r "    [A-Z][a-z]* " --include="*.hpp" $INTERFACES_DIR $SUBSYSTEMS_DIR | grep -v "virtual" | grep -v "explicit" | head -20 >> "$RESULT_FILE"

# 4. Suggestions de renommage
echo -e "\n=== Suggestions de renommage ===" >> "$RESULT_FILE"
echo "Si 'getProperty()' -> Renommer en 'property()'" >> "$RESULT_FILE"
echo "Si 'getValue()' -> Renommer en 'value()'" >> "$RESULT_FILE"
echo "Si 'modifyProperty()' -> Renommer en 'setProperty()'" >> "$RESULT_FILE"
echo "Si 'updateConfig()' -> Renommer en 'setConfig()'" >> "$RESULT_FILE"

echo "Analyse terminée. Résultats dans $RESULT_FILE"
