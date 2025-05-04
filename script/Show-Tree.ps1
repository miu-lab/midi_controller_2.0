<#
.SYNOPSIS
    Affiche une arborescence façon “tree” en chemins relatifs.

.PARAMETER Path
    Dossier racine (défaut : “.”).

.PARAMETER Exclude
    Noms de dossiers dont on veut afficher le nœud mais pas le contenu.

.EXAMPLE
    # Depuis C:\Projets, on liste ProjA en ignorant .pio et .git
    .\Show-Tree.ps1 -Path 'ProjA' -Exclude '.pio','.git'
#>

param(
    [string]   $Path    = '.',
    [string[]] $Exclude = '.pio'
)

# -- Fonction utilitaire : chemin relatif robuste (PS 5.1 & 7+)
function Get-RelativePath {
    param($Base, $Target)
    try   { return [System.IO.Path]::GetRelativePath($Base, $Target) }
    catch { return $Target -replace [regex]::Escape("$Base\"), '' }
}

function Show-Tree {
    param(
        [string]   $CurrentPath,
        [string]   $Prefix     = '',
        [string[]] $ExcludeDir,
        [string]   $BasePath   # pour calculer le relatif
    )

    $dirs  = Get-ChildItem -LiteralPath $CurrentPath -Directory | Sort-Object Name
    $files = Get-ChildItem -LiteralPath $CurrentPath -File      | Sort-Object Name
    $items = @($dirs + $files)

    for ($i = 0; $i -lt $items.Count; $i++) {
        $item    = $items[$i]
        $isLast  = $i -eq $items.Count - 1
        $branch  = if ($isLast) { '└───' } else { '├───' }
        $newPref = if ($isLast) { '    ' } else { '│   ' }

        Write-Host "$Prefix$branch$($item.Name)"

        if ($item.PSIsContainer -and $ExcludeDir -notcontains $item.Name) {
            Show-Tree -CurrentPath $item.FullName `
                      -Prefix      ($Prefix + $newPref) `
                      -ExcludeDir  $ExcludeDir `
                      -BasePath    $BasePath
        }
    }
}

# -- Point d’entrée
$cwd      = (Get-Location).ProviderPath
$rootAbs  = (Resolve-Path -LiteralPath $Path).ProviderPath
$rootRel  = (Get-RelativePath $cwd $rootAbs)

Write-Host $rootRel
Show-Tree -CurrentPath $rootAbs -ExcludeDir $Exclude -BasePath $rootAbs
