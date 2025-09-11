@echo off
echo ==================================================
echo PC MIDI Studio Extension - Build Script
echo ==================================================

set "BITWIG_JRE=C:\Program Files\Bitwig Studio 6.0 Beta 2\jre"
set "JAVAC=%BITWIG_JRE%\bin\javac.exe"
set "JAR=%BITWIG_JRE%\bin\jar.exe"

echo Using Bitwig JRE: %BITWIG_JRE%
echo.

REM Create build directories
echo Creating build directories...
if not exist "build" mkdir build
if not exist "dist" mkdir dist

REM Check if API exists
if not exist "lib\bitwig-extension-api.jar" (
    echo ERROR: Bitwig Extension API not found!
    echo Please run: curl -o lib/bitwig-extension-api.jar "https://maven.bitwig.com/com/bitwig/extension-api/18/extension-api-18.jar"
    pause
    exit /b 1
)

echo.
echo Compiling Java sources...
"%JAVAC%" -cp "lib\bitwig-extension-api.jar" -d build src\main\java\audio\petitechose\controller\*.java

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Compilation failed!
    pause
    exit /b 1
)

echo Compilation successful!
echo.

echo Creating JAR package...
REM Create manifest
echo Manifest-Version: 1.0 > build\MANIFEST.MF
echo Extension-Name: PC MIDI Studio >> build\MANIFEST.MF
echo Extension-Version: 1.0.0 >> build\MANIFEST.MF
echo Extension-Author: PetiteChose Audio >> build\MANIFEST.MF
echo. >> build\MANIFEST.MF

REM Create JAR
cd build
"%JAR%" cfm ..\dist\PCMidiStudio.jar MANIFEST.MF audio\
cd ..

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: JAR creation failed!
    pause
    exit /b 1
)

echo.
echo ==================================================
echo BUILD SUCCESSFUL!
echo ==================================================
echo Extension created: dist\PCMidiStudio.jar
echo.
echo To install:
echo 1. Copy dist\PCMidiStudio.jar to your Documents\Bitwig Studio\Extensions\ folder
echo 2. Restart Bitwig Studio or reload extensions
echo 3. Add controller in Settings ^> Controllers
echo.

REM Ask if user wants to install automatically
set /p INSTALL="Do you want to install the extension now? (Y/N): "
if /I "%INSTALL%"=="Y" (
    echo.
    echo Installing extension...
    if not exist "%USERPROFILE%\Documents\Bitwig Studio\Extensions" (
        mkdir "%USERPROFILE%\Documents\Bitwig Studio\Extensions"
    )
    copy "dist\PCMidiStudio.jar" "%USERPROFILE%\Documents\Bitwig Studio\Extensions\"
    echo Extension installed to: %USERPROFILE%\Documents\Bitwig Studio\Extensions\PCMidiStudio.jar
    echo Please restart Bitwig Studio or reload extensions.
)

echo.
pause