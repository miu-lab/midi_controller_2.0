@echo off
echo ===============================
echo Préparation de la Phase 8
echo ===============================
echo.
echo Compilation du projet MIDI Controller avec les changements préparatoires...
platformio run --environment dev
echo.
echo Exécution des tests...
platformio test --environment dev
echo.
echo Si la compilation et les tests ont réussi, la Phase 7 est complète!
echo Vous pouvez maintenant passer à la Phase 8 (suppression complète de ServiceLocator).
echo.
echo Voir le plan détaillé dans: docs\Phase8_Plan.md
echo.
pause
