#!/bin/bash
# generate_standardization_report.sh - Script pour générer un rapport complet de standardisation

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
CONFIG_DIR="$SRC_DIR/config"
ADAPTERS_DIR="$SRC_DIR/adapters"

# Créer dossier de rapports
mkdir -p "$PROJECT_ROOT/standardization_reports"

# Fichier de rapport
REPORT_FILE="$PROJECT_ROOT/standardization_reports/rapport_standardisation.md"

# En-tête du rapport
echo "# Rapport de standardisation du code - $(date +%Y-%m-%d)" > $REPORT_FILE
echo "" >> $REPORT_FILE
echo "## Résumé" >> $REPORT_FILE
echo "" >> $REPORT_FILE

# Compter les fichiers
TOTAL_FILES=$(find $SRC_DIR -name "*.hpp" -o -name "*.cpp" | wc -l)
echo "Total des fichiers: $TOTAL_FILES" >> $REPORT_FILE
echo "" >> $REPORT_FILE

# 1. Vérifier les suffixes _ pour les membres
echo "## 1. Membres de classe" >> $REPORT_FILE
echo "" >> $REPORT_FILE

NO_UNDERSCORE=$(grep -r "private:" --include="*.hpp" $SRC_DIR | grep -v "_;" | grep ";" | wc -l)
WITH_UNDERSCORE=$(grep -r "private:" --include="*.hpp" $SRC_DIR | grep "_;" | wc -l)
TOTAL_MEMBERS=$((NO_UNDERSCORE + WITH_UNDERSCORE))

PERCENT_STANDARDIZED=0
if [ $TOTAL_MEMBERS -gt 0 ]; then
  PERCENT_STANDARDIZED=$((WITH_UNDERSCORE * 100 / TOTAL_MEMBERS))
fi

echo "- Membres standardisés (avec suffixe _): $WITH_UNDERSCORE / $TOTAL_MEMBERS ($PERCENT_STANDARDIZED%)" >> $REPORT_FILE
echo "" >> $REPORT_FILE

# 2. Vérifier les interfaces (préfixe I)
echo "## 2. Interfaces" >> $REPORT_FILE
echo "" >> $REPORT_FILE

INTERFACES_WITH_I=$(find $CORE_DIR/domain/interfaces -name "I*.hpp" | wc -l)
INTERFACES_WITHOUT_I=$(find $CORE_DIR/domain/interfaces -name "*.hpp" | grep -v "^I" | wc -l)
TOTAL_INTERFACES=$((INTERFACES_WITH_I + INTERFACES_WITHOUT_I))

PERCENT_STANDARDIZED=0
if [ $TOTAL_INTERFACES -gt 0 ]; then
  PERCENT_STANDARDIZED=$((INTERFACES_WITH_I * 100 / TOTAL_INTERFACES))
fi

echo "- Interfaces standardisées (avec préfixe I): $INTERFACES_WITH_I / $TOTAL_INTERFACES ($PERCENT_STANDARDIZED%)" >> $REPORT_FILE
echo "" >> $REPORT_FILE

# 3. Vérifier la documentation Doxygen
echo "## 3. Documentation" >> $REPORT_FILE
echo "" >> $REPORT_FILE

FILES_WITH_DOXYGEN=$(grep -l "/**" $(find $SRC_DIR -name "*.hpp") | wc -l)
PERCENT_DOCUMENTED=0
if [ $TOTAL_FILES -gt 0 ]; then
  PERCENT_DOCUMENTED=$((FILES_WITH_DOXYGEN * 100 / TOTAL_FILES))
fi

echo "- Fichiers documentés avec Doxygen: $FILES_WITH_DOXYGEN / $TOTAL_FILES ($PERCENT_DOCUMENTED%)" >> $REPORT_FILE
echo "" >> $REPORT_FILE

# 4. Vérifier l'utilisation de Result
echo "## 4. Gestion des erreurs avec Result" >> $REPORT_FILE
echo "" >> $REPORT_FILE

METHODS_WITH_RESULT=$(grep -r "Result<" --include="*.hpp" $SRC_DIR | wc -l)
echo "- Méthodes utilisant Result: $METHODS_WITH_RESULT" >> $REPORT_FILE
echo "" >> $REPORT_FILE

# 5. Suggestions prioritaires
echo "## 5. Prochaines standardisations prioritaires" >> $REPORT_FILE
echo "" >> $REPORT_FILE

echo "Fichiers sans documentation Doxygen:" >> $REPORT_FILE
find $APP_DIR $CORE_DIR/domain/interfaces -name "*.hpp" | xargs grep -L "/\*\*" | head -5 >> $REPORT_FILE
echo "" >> $REPORT_FILE

echo "Fichiers avec membres sans suffixe _:" >> $REPORT_FILE
grep -r "private:" --include="*.hpp" $APP_DIR | grep -v "_;" | grep ";" | head -5 >> $REPORT_FILE
echo "" >> $REPORT_FILE

echo "Rapport généré dans $REPORT_FILE"
