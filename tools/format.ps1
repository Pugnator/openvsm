[CmdletBinding()]
param(
    [switch]$Check
)

$ErrorActionPreference = "Stop"
$repositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$patterns = @("*.c", "*.cc", "*.cpp", "*.cxx", "*.h", "*.hpp")

$trackedFiles = & git -C $repositoryRoot ls-files -- $patterns
if ($LASTEXITCODE -ne 0) {
    throw "Failed to enumerate tracked source files."
}

$sourceFiles = @(
    $trackedFiles |
        Where-Object {
            $_ -notmatch "^(externals/|model/cpp/log/)" -and
            $_ -ne "model/include/incbin.h"
        } |
        Sort-Object -Unique
)

if ($sourceFiles.Count -eq 0) {
    throw "No project-owned C or C++ files were found."
}

$clangFormat = (Get-Command clang-format -ErrorAction Stop).Source

Push-Location $repositoryRoot
try {
    if ($Check) {
        $failedFiles = @()
        foreach ($sourceFile in $sourceFiles) {
            & $clangFormat --dry-run --Werror --style=file -- $sourceFile
            if ($LASTEXITCODE -ne 0) {
                $failedFiles += $sourceFile
            }
        }

        if ($failedFiles.Count -ne 0) {
            throw "Formatting check failed for: $($failedFiles -join ', ')"
        }

        Write-Output "Formatting check passed for $($sourceFiles.Count) files."
    }
    else {
        foreach ($sourceFile in $sourceFiles) {
            & $clangFormat -i --style=file -- $sourceFile
            if ($LASTEXITCODE -ne 0) {
                throw "clang-format failed for $sourceFile"
            }
        }

        Write-Output "Formatted $($sourceFiles.Count) files."
    }
}
finally {
    Pop-Location
}
