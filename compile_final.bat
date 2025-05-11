@echo off
echo ===============================
echo Phase 8 - Compilation finale
echo ===============================
echo.
echo Compilation du projet MIDI Controller sans ServiceLocator...
platformio run --environment dev
echo.
echo Exécution des tests...
platformio test --environment dev
echo.
echo Si la compilation et les tests ont réussi, la Phase 8 est complète!
echo.
echo La migration vers le nouveau système d'injection de dépendances est terminée.
echo.
pause