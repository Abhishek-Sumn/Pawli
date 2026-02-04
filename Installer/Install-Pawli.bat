@echo off
:: Pawli Installer - Double-click to install
:: Auto-start with Windows is enabled by default

echo.
echo   Starting Pawli Installer...
echo.

powershell -ExecutionPolicy Bypass -File "%~dp0install.ps1"

pause
