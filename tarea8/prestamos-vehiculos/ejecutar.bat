@echo off
setlocal
cd /d "%~dp0"

set "MSYS=C:\msys64"
if not exist "%MSYS%\mingw64\bin\gcc.exe" (
  echo No se encontro MSYS2 en %MSYS%.
  echo Instale MSYS2 y los paquetes indicados en README.md.
  pause
  exit /b 1
)

set "PATH=%MSYS%\mingw64\bin;%PATH%"
set "MSYSTEM=MINGW64"
set "GSETTINGS_SCHEMA_DIR=%MSYS%\mingw64\share\glib-2.0\schemas"
set "XDG_DATA_DIRS=%MSYS%\mingw64\share"

if exist "build\autocredito.exe" goto run

echo Compilando por primera vez...
set "PROY=%CD:\=/%"
"%MSYS%\usr\bin\bash.exe" -lc "cd '%PROY%' && make"
if errorlevel 1 (
  pause
  exit /b 1
)

:run
start "" "build\autocredito.exe" %*
