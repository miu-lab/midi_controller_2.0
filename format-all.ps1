# format-all.ps1 - Script pour formater tous les fichiers C/C++ dans le dossier src

# Utiliser le répertoire où se trouve le script comme chemin du projet
$SCRIPT_PATH = Split-Path -Parent $MyInvocation.MyCommand.Definition
$PROJECT_PATH = $SCRIPT_PATH

Write-Host "Chemin du projet détecté : $PROJECT_PATH" -ForegroundColor Cyan

# Vérifiez si clang-format est disponible
try {
    $clangVersion = clang-format --version
    Write-Host "Utilisation de clang-format: $clangVersion"
}
catch {
    Write-Host "Erreur: clang-format n'est pas accessible dans le PATH" -ForegroundColor Red
    Write-Host "Assurez-vous que clang-format est installé et ajouté au PATH Windows" -ForegroundColor Red
    exit 1
}

# Vérifier que le dossier src existe
$srcPath = Join-Path -Path $PROJECT_PATH -ChildPath "src"
if (-not (Test-Path -Path $srcPath -PathType Container)) {
    Write-Host "Erreur: Le dossier 'src' n'existe pas dans $PROJECT_PATH" -ForegroundColor Red
    exit 1
}

# Compteurs pour les statistiques
$totalFiles = 0
$successFiles = 0
$errorFiles = 0

# Récupérer tous les fichiers C/C++ dans le dossier src
Write-Host "Recherche des fichiers à formater dans $srcPath..."
$files = Get-ChildItem -Path $srcPath -Recurse -Include "*.cpp", "*.hpp", "*.h", "*.c" -File

# Sortir si aucun fichier trouvé
if ($files.Count -eq 0) {
    Write-Host "Aucun fichier C/C++ trouvé dans le dossier $srcPath" -ForegroundColor Yellow
    exit 0
}

# Afficher le nombre de fichiers trouvés
Write-Host "Formatage de $($files.Count) fichiers..." -ForegroundColor Cyan

# Formater chaque fichier
foreach ($file in $files) {
    $totalFiles++
    $relativePath = $file.FullName.Substring($PROJECT_PATH.Length + 1)
    Write-Host "Formatage de $relativePath..." -NoNewline
    
    try {
        # Appliquer clang-format au fichier
        & clang-format -i $file.FullName
        
        if ($LASTEXITCODE -eq 0) {
            Write-Host " OK" -ForegroundColor Green
            $successFiles++
        } else {
            Write-Host " ERREUR (code $LASTEXITCODE)" -ForegroundColor Red
            $errorFiles++
        }
    }
    catch {
        Write-Host " ERREUR: $_" -ForegroundColor Red
        $errorFiles++
    }
}

# Afficher un résumé
Write-Host "`n=== Résumé du formatage ===" -ForegroundColor Cyan
Write-Host "Total de fichiers traités: $totalFiles" -ForegroundColor White
Write-Host "Fichiers formatés avec succès: $successFiles" -ForegroundColor Green
if ($errorFiles -gt 0) {
    Write-Host "Fichiers en erreur: $errorFiles" -ForegroundColor Red
}

Write-Host "`nFormatage terminé !" -ForegroundColor Cyan