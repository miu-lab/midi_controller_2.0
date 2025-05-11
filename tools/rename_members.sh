#!/bin/bash
# rename_members.sh - Script pour standardiser les noms des membres de classe

# Dossiers prioritaires pour la standardisation
PRIORITY_DIRS=(
  "src/app/subsystems"
  "src/app/di"
  "src/core/domain/interfaces"
)

# Créer un répertoire pour les backups
mkdir -p code_backups

echo "Démarrage du processus de renommage des membres..."

# Traiter chaque dossier prioritaire
for dir in "${PRIORITY_DIRS[@]}"; do
  echo "Traitement du dossier: $dir"
  
  # Chercher les fichiers .hpp
  for file in $(find "$dir" -name "*.hpp"); do
    echo "  - Traitement de $file"
    
    # Créer une sauvegarde
    backup_file="code_backups/$(basename "$file").bak"
    cp "$file" "$backup_file"
    
    # Identifier les déclarations de membres sans suffixe _
    # Complexe à faire en bash, une solution basique:
    
    # 1. Trouver les sections private: et protected:
    # 2. Identifier les lignes qui déclarent des membres sans suffixe _
    # 3. Ajouter le suffixe _ aux noms de membres
    
    # Cette solution est simplifiée et nécessiterait un outil plus robuste
    # comme un parser C++ pour une solution complète
    
    # Pour l'instant, on peut faire un remplacement basique pour les cas simples
    sed -i -E 's/private:([^;]*) ([a-zA-Z0-9]+);/private:\1 \2_;/g' "$file"
    sed -i -E 's/protected:([^;]*) ([a-zA-Z0-9]+);/protected:\1 \2_;/g' "$file"
    
    # Comparer pour voir les changements
    echo "    Changements:"
    diff -u "$backup_file" "$file" | grep -E "^\+|^\-" | head -5
  done
done

echo "Processus de renommage terminé. Vérifiez les modifications et adaptez les fichiers .cpp correspondants manuellement."
