# Pawli Desktop Pet - Installer Script
# Run with: powershell -ExecutionPolicy Bypass -File install.ps1

param(
    [switch]$NoStartup,      # Don't add to Windows startup
    [switch]$Uninstall,      # Remove Pawli
    [switch]$Silent          # No prompts
)

$AppName = "Pawli"
$InstallPath = "$env:APPDATA\Pawli"
$ExeName = "Pawli.exe"
$RegPath = "HKCU:\Software\Microsoft\Windows\CurrentVersion\Run"

function Write-Banner {
    Write-Host ""
    Write-Host "  ========================================" -ForegroundColor Cyan
    Write-Host "         Pawli Desktop Pet          " -ForegroundColor Cyan
    Write-Host "     Your cute companion on desktop     " -ForegroundColor Cyan
    Write-Host "  ========================================" -ForegroundColor Cyan
    Write-Host ""
}

function Install-Pawli {
    Write-Banner
    
    # Show Controls
    Write-Host "  Controls:" -ForegroundColor Yellow
    Write-Host "  [Left Click]  " -NoNewline -ForegroundColor White; Write-Host " Interact / Reaction" -ForegroundColor Gray
    Write-Host "  [Right Drag]  " -NoNewline -ForegroundColor White; Write-Host " Move Pawli" -ForegroundColor Gray
    Write-Host "  [5x R-Click]  " -NoNewline -ForegroundColor White; Write-Host " Exit App" -ForegroundColor Gray
    Write-Host ""

    if (!$Silent) {
        $confirm = Read-Host "  Do you want to install Pawli? (Y/n)"
        if ($confirm -eq "n" -or $confirm -eq "N") {
            Write-Host "  Installation cancelled." -ForegroundColor Yellow
            return $false
        }
    }
    Write-Host ""
    
    # Create install directory
    if (!(Test-Path $InstallPath)) {
        New-Item -ItemType Directory -Path $InstallPath -Force | Out-Null
        Write-Host "  [+] Created install directory" -ForegroundColor Green
    }
    
    # Copy files
    $SourcePath = Split-Path -Parent $PSScriptRoot
    if (Test-Path "$SourcePath\bin\$ExeName") {
        Copy-Item "$SourcePath\bin\$ExeName" "$InstallPath\" -Force
        Write-Host "  [+] Copied executable" -ForegroundColor Green
    } else {
        Write-Host "  [!] Executable not found. Build the project first!" -ForegroundColor Red
        return $false
    }
    
    # Copy Assets folder
    if (Test-Path "$SourcePath\Assets") {
        Copy-Item "$SourcePath\Assets" "$InstallPath\" -Recurse -Force
        Write-Host "  [+] Copied assets" -ForegroundColor Green
    }
    
    # Add to Windows startup (default: ON)
    if (!$NoStartup) {
        $ExeFullPath = "$InstallPath\$ExeName"
        $Value = "`"$ExeFullPath`""
        Set-ItemProperty -Path $RegPath -Name $AppName -Value $Value -Force
        Write-Host "  [+] Added to Windows startup" -ForegroundColor Green
    } else {
        Write-Host "  [-] Skipped Windows startup (--NoStartup)" -ForegroundColor Yellow
    }
    
    # Create Start Menu shortcut
    $StartMenuPath = "$env:APPDATA\Microsoft\Windows\Start Menu\Programs"
    $ShortcutPath = "$StartMenuPath\$AppName.lnk"
    
    try {
        $WshShell = New-Object -ComObject WScript.Shell
        $Shortcut = $WshShell.CreateShortcut($ShortcutPath)
        $Shortcut.TargetPath = "$InstallPath\$ExeName"
        $Shortcut.WorkingDirectory = $InstallPath
        $Shortcut.Description = "Pawli Desktop Pet"
        $Shortcut.Save()
        Write-Host "  [+] Created Start Menu shortcut" -ForegroundColor Green
    } catch {
        Write-Host "  [!] Failed to create shortcut: $_" -ForegroundColor Red
    }
    
    Write-Host ""
    Write-Host "  [OK] Installation complete!" -ForegroundColor Green
    Write-Host ""
    Write-Host "  Launch Pawli from Start Menu or run:" -ForegroundColor White
    Write-Host "  $InstallPath\$ExeName" -ForegroundColor Cyan
    Write-Host ""
    
    if (!$Silent) {
        $launch = Read-Host "  Launch Pawli now? (Y/n)"
        if ($launch -ne "n" -and $launch -ne "N") {
            try {
                Start-Process "$InstallPath\$ExeName"
            } catch {
                Write-Host "  [!] Failed to launch: $_" -ForegroundColor Red
            }
        }
    }
    
    return $true
}

function Uninstall-Pawli {
    Write-Banner
    Write-Host "  Uninstalling Pawli..." -ForegroundColor Yellow
    
    # Remove from startup
    if (Get-ItemProperty -Path $RegPath -Name $AppName -ErrorAction SilentlyContinue) {
        Remove-ItemProperty -Path $RegPath -Name $AppName -Force
        Write-Host "  [+] Removed from Windows startup" -ForegroundColor Green
    }
    
    # Remove Start Menu shortcut
    $ShortcutPath = "$env:APPDATA\Microsoft\Windows\Start Menu\Programs\$AppName.lnk"
    if (Test-Path $ShortcutPath) {
        Remove-Item $ShortcutPath -Force
        Write-Host "  [+] Removed Start Menu shortcut" -ForegroundColor Green
    }
    
    # Kill running process
    Get-Process -Name "Pawli" -ErrorAction SilentlyContinue | Stop-Process -Force
    
    # Remove install directory
    if (Test-Path $InstallPath) {
        Remove-Item $InstallPath -Recurse -Force
        Write-Host "  [+] Removed install directory" -ForegroundColor Green
    }
    
    Write-Host ""
    Write-Host "  [OK] Uninstall complete!" -ForegroundColor Green
    Write-Host ""
}

# Main
if ($Uninstall) {
    Uninstall-Pawli
} else {
    Install-Pawli
}
