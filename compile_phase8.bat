@echo off
echo ===============================
echo Phase 8 - Compilation
echo ===============================
echo.
echo Compilation du projet MIDI Controller sans ServiceLocator...
platformio run --environment dev
echo.
pause