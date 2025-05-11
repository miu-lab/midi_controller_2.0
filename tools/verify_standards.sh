#!/bin/bash
# verify_standards.sh - Script de vérification complète des standards

# Déterminer le chemin de base du projet
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Vérifier si le répertoire source existe
if [ ! -d "$PROJECT_ROOT/src" ]; then
    echo "ERREUR: Le répertoire src n'existe pas au chemin $PROJECT_ROOT/src"
    echo "Le script doit être exécuté depuis la racine du projet ou le répertoire tools"
    exit 1
fi

# Configuration
SRC_DIR="$PROJECT_ROOT/src"
APP_DIR="$SRC_DIR/app"
CORE_DIR="$SRC_DIR/core"
SUBSYSTEMS_DIR="$APP_DIR/subsystems"
INTERFACES_DIR="$CORE_DIR/domain/interfaces"
DI_DIR="$APP_DIR/di"

# Créer répertoire pour les résultats
mkdir -p "$PROJECT_ROOT/verification_results"

# Fichier de rapport
REPORT_FILE="$PROJECT_ROOT/verification_results/standards_verification.md"

# En-tête du rapport
echo "# Vérification des standards - $(date +%Y-%m-%d)" > $REPORT_FILE
echo "" >> $REPORT_FILE

# 1. Vérifier les membres de classe (suffixe _)
echo "## 1. Vérification des membres de classe" >> $REPORT_FILE
echo "" >> $REPORT_FILE
echo "Fichiers avec membres sans suffixe _ :" >> $REPORT_FILE
echo "" >> $REPORT_FILE
find $SUBSYSTEMS_DIR $DI_DIR -name "*.hpp" -exec grep -l "private:" {} \; | while read file; do
  if grep -q "private:" "$file"; then
    if grep -q "private:.*[^_];" "$file"; then
      echo "- $file" >> $REPORT_FILE
      grep -n "private:.*[^_];" "$file" | head -3 >> $REPORT_FILE
    fi
  fi
done
echo "" >> $REPORT_FILE

# 2. Vérifier la documentation Doxygen
echo "## 2. Vérification de la documentation" >> $REPORT_FILE
echo "" >> $REPORT_FILE
echo "Fichiers sans documentation Doxygen :" >> $REPORT_FILE
echo "" >> $REPORT_FILE
find $SUBSYSTEMS_DIR $INTERFACES_DIR $DI_DIR -name "*.hpp" | while read file; do
  if ! grep -q "/\*\*" "$file"; then
    echo "- $file" >> $REPORT_FILE
  fi
done
echo "" >> $REPORT_FILE

# 3. Vérifier les noms des méthodes
echo "## 3. Vérification des noms de méthodes" >> $REPORT_FILE
echo "" >> $REPORT_FILE
echo "Getters avec préfixe 'get' :" >> $REPORT_FILE
echo "" >> $REPORT_FILE
grep -r "get[A-Z]" --include="*.hpp" $SUBSYSTEMS_DIR $INTERFACES_DIR | grep "virtual" | head -10 >> $REPORT_FILE
echo "" >> $REPORT_FILE

echo "Méthodes avec casse incorrecte (commençant par majuscule) :" >> $REPORT_FILE
echo "" >> $REPORT_FILE
grep -r "    [A-Z][a-z]* " --include="*.hpp" $SUBSYSTEMS_DIR $INTERFACES_DIR | grep -v "virtual" | grep -v "explicit" | head -10 >> $REPORT_FILE
echo "" >> $REPORT_FILE

# 4. Vérifier l'utilisation de Result
echo "## 4. Utilisation de Result<T, E>" >> $REPORT_FILE
echo "" >> $REPORT_FILE
echo "Fichiers utilisant Result :" >> $REPORT_FILE
echo "" >> $REPORT_FILE
grep -l "Result<" $(find $SRC_DIR -name "*.hpp") >> $REPORT_FILE
echo "" >> $REPORT_FILE

# 5. Bilan et recommandations
echo "## 5. Bilan et recommandations" >> $REPORT_FILE
echo "" >> $REPORT_FILE

# Compter les fichiers conformes
TOTAL_FILES=$(find $SUBSYSTEMS_DIR $INTERFACES_DIR $DI_DIR -name "*.hpp" | wc -l)
FILES_WITH_DOXYGEN=$(grep -l "/\*\*" $(find $SUBSYSTEMS_DIR $INTERFACES_DIR $DI_DIR -name "*.hpp") | wc -l)
FILES_WITH_RESULT=$(grep -l "Result<" $(find $SUBSYSTEMS_DIR $INTERFACES_DIR $DI_DIR -name "*.hpp") | wc -l)

echo "**Bilan :**" >> $REPORT_FILE
echo "- Total des fichiers vérifiés : $TOTAL_FILES" >> $REPORT_FILE
echo "- Fichiers avec documentation Doxygen : $FILES_WITH_DOXYGEN" >> $REPORT_FILE
echo "- Fichiers utilisant Result : $FILES_WITH_RESULT" >> $REPORT_FILE
echo "" >> $REPORT_FILE

echo "**Recommandations prioritaires :**" >> $REPORT_FILE
echo "1. Compléter la documentation Doxygen pour tous les fichiers d'interface" >> $REPORT_FILE
echo "2. Standardiser les noms de méthodes (getters sans 'get', setters avec 'set')" >> $REPORT_FILE
echo "3. Ajouter le suffixe _ à tous les membres de classe" >> $REPORT_FILE
echo "4. Étendre l'utilisation de Result<T, E> aux autres sous-systèmes" >> $REPORT_FILE

echo "Vérification terminée. Rapport disponible dans $REPORT_FILE"
