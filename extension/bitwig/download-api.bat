@echo off

REM Script pour télécharger l'API Bitwig
set API_VERSION=25
set API_DIR=api\%API_VERSION%

echo Telechargement de l'API Bitwig version %API_VERSION%...

REM Créer le dossier si nécessaire
if not exist "%API_DIR%" mkdir "%API_DIR%"

REM URLs des fichiers
set JAR_URL=https://maven.bitwig.com/com/bitwig/extension-api/%API_VERSION%/extension-api-%API_VERSION%.jar
set SOURCES_URL=https://maven.bitwig.com/com/bitwig/extension-api/%API_VERSION%/extension-api-%API_VERSION%-sources.jar

REM Télécharger les fichiers avec PowerShell
echo Telechargement du JAR principal...
powershell -Command "Invoke-WebRequest -Uri '%JAR_URL%' -OutFile '%API_DIR%\extension-api-%API_VERSION%.jar'"

echo Telechargement des sources...
powershell -Command "Invoke-WebRequest -Uri '%SOURCES_URL%' -OutFile '%API_DIR%\extension-api-%API_VERSION%-sources.jar'"

echo Telechargement termine !
echo Les fichiers sont dans : %API_DIR%