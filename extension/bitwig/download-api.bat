@echo off
echo Downloading Bitwig Extension API...
curl -o lib/bitwig-extension-api.jar "https://maven.bitwig.com/com/bitwig/extension-api/18/extension-api-18.jar"
if %ERRORLEVEL% EQU 0 (
    echo API downloaded successfully to lib/bitwig-extension-api.jar
) else (
    echo Failed to download API. You can also use Maven or Gradle instead.
)
pause