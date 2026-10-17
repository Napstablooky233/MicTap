@echo off
rem ---------------------------------------------------------------------------
rem  MicTap build script (MinGW-w64 / w64devkit)
rem
rem  Usage:  build.cmd            release build -> build\MicTap.exe
rem          build.cmd debug      console subsystem, keeps stderr visible
rem
rem  Requires g++ and windres on PATH, or set MINGW_BIN to the bin directory.
rem ---------------------------------------------------------------------------
setlocal enabledelayedexpansion
cd /d "%~dp0"

if "%MINGW_BIN%"=="" set "MINGW_BIN=D:\mingw\w64devkit\bin"
set "CXX=%MINGW_BIN%\x86_64-w64-mingw32-g++.exe"
set "WINDRES=%MINGW_BIN%\x86_64-w64-mingw32-windres.exe"

if not exist "%CXX%" (
    echo [error] g++ not found: %CXX%
    echo         Install w64devkit or set MINGW_BIN to its bin directory.
    exit /b 1
)

set "SUBSYS=-mwindows"
if /i "%~1"=="debug" set "SUBSYS=-mconsole"

if not exist build mkdir build

echo [1/2] resources
"%WINDRES%" -I src src\MicTap.rc -O coff -o build\MicTap.res || exit /b 1

echo [2/2] compile + link
"%CXX%" src\MicTap.cpp build\MicTap.res ^
    -std=c++17 -O2 -static %SUBSYS% -municode ^
    -lole32 -luuid -loleaut32 -lmmdevapi -lshell32 -lgdi32 -luser32 ^
    -o build\MicTap.exe || exit /b 1

for %%F in (build\MicTap.exe) do echo [ok] %%~fF  (%%~zF bytes)
endlocal
