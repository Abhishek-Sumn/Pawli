@echo off
setlocal

set "OUT_DIR=Pawli_Release"

echo Packaging Pawli for release...

REM 1. Clean output directory
if exist "%OUT_DIR%" rmdir /S /Q "%OUT_DIR%"
mkdir "%OUT_DIR%"

REM 2. Copy binaries
mkdir "%OUT_DIR%\bin"
if exist "bin\Pawli.exe" (
    copy "bin\Pawli.exe" "%OUT_DIR%\bin\" >nul
    echo [OK] Copied executable
) else (
    echo [ERROR] bin\Pawli.exe not found! Build first.
    exit /b 1
)

REM 3. Copy assets
mkdir "%OUT_DIR%\Assets"
xcopy "Assets" "%OUT_DIR%\Assets" /E /I /Y >nul
echo [OK] Copied assets

REM 4. Copy installer scripts
mkdir "%OUT_DIR%\Installer"
copy "Installer\install.ps1" "%OUT_DIR%\Installer\" >nul
copy "Installer\Install-Pawli.bat" "%OUT_DIR%\Installer\" >nul
copy "Installer\Uninstall-Pawli.bat" "%OUT_DIR%\Installer\" >nul
echo [OK] Copied installer scripts

REM 5. Create root level launchers for convenience
echo @echo off > "%OUT_DIR%\Install_Pawli.bat"
echo call "Installer\Install-Pawli.bat" >> "%OUT_DIR%\Install_Pawli.bat"

echo @echo off > "%OUT_DIR%\Uninstall_Pawli.bat"
echo call "Installer\Uninstall-Pawli.bat" >> "%OUT_DIR%\Uninstall_Pawli.bat"

REM 6. Create README
echo ========================================== > "%OUT_DIR%\README.txt"
echo            PAWLI DESKTOP PET               >> "%OUT_DIR%\README.txt"
echo ========================================== >> "%OUT_DIR%\README.txt"
echo. >> "%OUT_DIR%\README.txt"
echo To install Pawli: >> "%OUT_DIR%\README.txt"
echo 1. Double-click "Install_Pawli.bat" >> "%OUT_DIR%\README.txt"
echo 2. Follow the on-screen prompts. >> "%OUT_DIR%\README.txt"
echo. >> "%OUT_DIR%\README.txt"
echo To uninstall: >> "%OUT_DIR%\README.txt"
echo Run "Uninstall_Pawli.bat" >> "%OUT_DIR%\README.txt"
echo. >> "%OUT_DIR%\README.txt"
echo You can move this entire folder anywhere, >> "%OUT_DIR%\README.txt"
echo but keep all files together inside it. >> "%OUT_DIR%\README.txt"

echo.
echo ==========================================
echo  Release created in: %OUT_DIR%
echo  Zip this folder to share it!
echo ==========================================
echo.

pause
