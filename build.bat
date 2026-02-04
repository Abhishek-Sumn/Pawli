@echo off
setlocal

REM === Initialize MSVC environment ===
call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" >nul
if errorlevel 1 (
    echo Failed to initialize Visual Studio environment
    exit /b 1
)

REM === Output directory ===
if not exist "bin" mkdir bin

echo Compiling Pawli with Kitty Sprites...

cl /nologo ^
   /std:c++20 ^
   /EHsc ^
   /W3 ^
   /Od ^
   /Zi ^
   /MDd ^
   /DUNICODE /D_UNICODE ^
   /I. ^
   /Fe:bin\Pawli.exe ^
   Core\App.cpp ^
   Core\Window.cpp ^
   Core\RenderContext.cpp ^
   Assets\SpriteAtlas.cpp ^
   Assets\SpriteLoader.cpp ^
   Input\InputHandler.cpp ^
   Pet\PetStateMachine.cpp ^
   Pet\PetController.cpp ^
   Pet\PetAnimator.cpp ^
   Desktop\MonitorManager.cpp ^
   Desktop\VirtualDesktop.cpp ^
   Desktop\DesktopTracker.cpp ^
   /link ^
   user32.lib ^
   gdi32.lib ^
   d2d1.lib ^
   ole32.lib ^
   windowscodecs.lib ^
   shcore.lib ^
   /SUBSYSTEM:WINDOWS

if errorlevel 1 goto :build_failed

echo.
echo Build successful!
echo Run: bin\Pawli.exe
echo.
echo Features:
echo   - Cute animated kitty!
echo   - Right-click drag to move
echo   - Left-click for reaction
echo   - Walks randomly
echo.
exit /b 0

:build_failed
echo.
echo Build failed!
echo.
exit /b 1
