@echo off
:: Pawli Uninstaller - Double-click to remove

echo.
echo   Starting Pawli Uninstaller...
echo.

powershell -ExecutionPolicy Bypass -File "%~dp0install.ps1" -Uninstall

pause
