@echo off
echo ===================================
echo Compilation et tests MIDI Controller
echo ===================================

echo.
echo 1. Compilation du projet...
platformio run

if %ERRORLEVEL% NEQ 0 (
  echo Erreur de compilation!
  exit /b %ERRORLEVEL%
)

echo.
echo 2. Execution des tests...
platformio test

if %ERRORLEVEL% NEQ 0 (
  echo Certains tests ont échoué!
  exit /b %ERRORLEVEL%
)

echo.
echo Compilation et tests réussis!
echo ===================================
