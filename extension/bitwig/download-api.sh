#!/bin/bash

# Script pour télécharger l'API Bitwig
API_VERSION=25
API_DIR="api/$API_VERSION"

echo "Téléchargement de l'API Bitwig version $API_VERSION..."

# Créer le dossier si nécessaire
mkdir -p "$API_DIR"

# URLs des fichiers
JAR_URL="https://maven.bitwig.com/com/bitwig/extension-api/$API_VERSION/extension-api-$API_VERSION.jar"
SOURCES_URL="https://maven.bitwig.com/com/bitwig/extension-api/$API_VERSION/extension-api-$API_VERSION-sources.jar"

# Télécharger les fichiers
echo "Téléchargement du JAR principal..."
curl -L "$JAR_URL" -o "$API_DIR/extension-api-$API_VERSION.jar"

echo "Téléchargement des sources..."
curl -L "$SOURCES_URL" -o "$API_DIR/extension-api-$API_VERSION-sources.jar"

echo "Téléchargement terminé !"
echo "Les fichiers sont dans : $API_DIR"