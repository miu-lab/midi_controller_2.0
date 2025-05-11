@echo off
echo ===============================
echo Finalisation de la Phase 7
echo ===============================
echo.
echo Compilation du projet MIDI Controller...
platformio run --environment dev
echo.
echo Exécution des tests...
platformio test --environment dev
echo.
echo Si la compilation et les tests ont réussi, la Phase 7 est complète!
echo Vous pouvez maintenant commencer à utiliser la nouvelle architecture.
echo.
echo Voir les documents de migration et guides d'utilisation dans le dossier docs/
echo  - docs/MigrationGuide.md
echo  - docs/DependencyInjection.md
echo.
pause
