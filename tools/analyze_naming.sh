#!/bin/bash
# analyze_naming.sh - Script pour analyser le respect des conventions de nommage

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
APP_DIR="$SRC_DIR/app"
CORE_DIR="$SRC_DIR/core"
DI_DIR="$APP_DIR/di"
SUBSYSTEMS_DIR="$APP_DIR/subsystems"
INTERFACES_DIR="$CORE_DIR/domain/interfaces"

# Créer le dossier de résultats
mkdir -p "$PROJECT_ROOT/analysis_results"

RESULT_FILE="$PROJECT_ROOT/analysis_results/naming_analysis.txt"
echo "=== Analyse des conventions de nommage ===" > "$RESULT_FILE"

# 1. Rechercher les membres de classe sans suffixe _
echo "=== Membres de classe sans suffixe _ ===" >> "$RESULT_FILE"
grep -r "private:" --include="*.hpp" $SRC_DIR | grep -v "_;" | grep ";" | head -20 >> "$RESULT_FILE"

# 2. Rechercher les interfaces sans préfixe I
echo -e "\n=== Interfaces sans préfixe I ===" >> "$RESULT_FILE"
find $INTERFACES_DIR -name "*.hpp" | grep -v "^I" | xargs basename -a 2>/dev/null >> "$RESULT_FILE"

# 3. Vérifier les fichiers dont le nom ne correspond pas à la classe
echo -e "\n=== Fichiers non conformes ===" >> "$RESULT_FILE"
for file in $(find $SRC_DIR -name "*.hpp"); do
  className=$(basename "$file" .hpp)
  grep -q "class $className" "$file" || echo "$file: Class name mismatch" >> "$RESULT_FILE"
done

# 4. Vérifier les méthodes get/set qui ne suivent pas les conventions
echo -e "\n=== Méthodes get/set non conformes ===" >> "$RESULT_FILE"
grep -r "get[A-Z]" --include="*.hpp" $SRC_DIR | grep -v "get_" | head -20 >> "$RESULT_FILE"

echo "Analyse terminée. Résultats dans $RESULT_FILE"
