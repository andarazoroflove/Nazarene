@echo off
setlocal enabledelayedexpansion

echo ======================================================================
echo  Building: "The Nazarene: Way of the Cross" (Mission 1)
echo ======================================================================

:: Add toolchain paths if present
if exist "C:\Users\adam\code\w64devkit\bin" (
    set "PATH=C:\Users\adam\code\w64devkit\bin;C:\Users\adam\AppData\Local\Programs\Python\Python312\Scripts;!PATH!"
)

:: Check for CMake
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake was not found in PATH!
    echo Please ensure CMake is installed and available in PATH.
    pause
    exit /b 1
)

:: Check for generator: Ninja or MinGW Makefiles or MSVC
set GENERATOR=
where ninja >nul 2>nul
if %ERRORLEVEL% equ 0 (
    set "GENERATOR=-G Ninja"
) else (
    where mingw32-make >nul 2>nul
    if %ERRORLEVEL% equ 0 (
        set "GENERATOR=-G \"MinGW Makefiles\""
    )
)

echo [1/3] Configuring project with CMake...
cmake -B build %GENERATOR% -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo [2/3] Building executable...
cmake --build build --config Release
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Compilation failed!
    pause
    exit /b 1
)

echo.
echo [3/3] Copying executable to root folder...
if exist "build\Nazarene.exe" (
    copy /y "build\Nazarene.exe" "Nazarene.exe" >nul
) else if exist "build\Release\Nazarene.exe" (
    copy /y "build\Release\Nazarene.exe" "Nazarene.exe" >nul
)

echo.
echo ======================================================================
echo  BUILD SUCCESSFUL!
echo  Executable ready: Nazarene.exe
echo ======================================================================
echo.

