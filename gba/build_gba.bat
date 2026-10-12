@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

echo ======================================================================
echo  Building: "The Nazarene: Way of the Cross" (Game Boy Advance Version)
echo ======================================================================

set "TOOLDIR=C:\Users\adam\code\arm-toolchain\arm-gnu-toolchain-13.3.rel1-mingw-w64-i686-arm-none-eabi\bin"
if not exist "%TOOLDIR%\arm-none-eabi-gcc.exe" (
    where arm-none-eabi-gcc.exe >nul 2>nul
    if %ERRORLEVEL% equ 0 (
        set "TOOLDIR="
    ) else (
        echo [ERROR] arm-none-eabi-gcc not found!
        exit /b 1
    )
)

set "CC=%TOOLDIR%\arm-none-eabi-gcc.exe"
if "%TOOLDIR%"=="" set "CC=arm-none-eabi-gcc"
set "OBJCOPY=%TOOLDIR%\arm-none-eabi-objcopy.exe"
if "%TOOLDIR%"=="" set "OBJCOPY=arm-none-eabi-objcopy"

echo [1/4] Compiling GBA assembly and C source files...
"%CC%" -mcpu=arm7tdmi -mthumb-interwork -c crt0.s -o crt0.o
if %ERRORLEVEL% neq 0 goto error
"%CC%" -mcpu=arm7tdmi -mthumb-interwork -O2 -I include -c src/gba_audio.c -o gba_audio.o
if %ERRORLEVEL% neq 0 goto error
"%CC%" -mcpu=arm7tdmi -mthumb-interwork -O2 -I include -c src/gba_graphics.c -o gba_graphics.o
if %ERRORLEVEL% neq 0 goto error
"%CC%" -mcpu=arm7tdmi -mthumb-interwork -O2 -I include -c src/gba_game.c -o gba_game.o
if %ERRORLEVEL% neq 0 goto error
"%CC%" -mcpu=arm7tdmi -mthumb-interwork -O2 -I include -c src/main.c -o main.o
if %ERRORLEVEL% neq 0 goto error

echo.
echo [2/4] Linking GBA ELF binary...
"%CC%" -nostartfiles -T gba.ld crt0.o main.o gba_game.o gba_graphics.o gba_audio.o -lc -lm -lgcc -o Nazarene.elf
if %ERRORLEVEL% neq 0 goto error

echo.
echo [3/4] Extracting raw binary Game Pak ROM...
"%OBJCOPY%" -O binary Nazarene.elf Nazarene.gba
if %ERRORLEVEL% neq 0 goto error

echo.
echo [4/4] Validating and patching Nintendo GBA header...
if exist "tools\gbafix.exe" (
    "tools\gbafix.exe" Nazarene.gba -tNAZARENE -cANZE -m01
) else if exist "..\gbafix.exe" (
    "..\gbafix.exe" Nazarene.gba -tNAZARENE -cANZE -m01
) else if exist "gbafix.exe" (
    "gbafix.exe" Nazarene.gba -tNAZARENE -cANZE -m01
)

python -c "with open('Nazarene.gba','rb') as f: d=f.read(); open('Nazarene-padded.gba','wb').write(d + b'\xFF'*(4*1024*1024 - len(d)))"
if exist "tools\gbafix.exe" (
    "tools\gbafix.exe" Nazarene-padded.gba -tNAZARENE -cANZE -m01
)

copy /y Nazarene.gba "..\Nazarene.gba" >nul
copy /y Nazarene-padded.gba "..\Nazarene-padded.gba" >nul

echo.
echo ======================================================================
echo  GBA BUILD SUCCESSFUL!
echo  Game Boy Advance ROM ready: Nazarene.gba
echo  Padded SuperCard ROM ready: Nazarene-padded.gba (4MB)
echo ======================================================================
goto end

:error
echo [ERROR] GBA compilation failed!
exit /b 1

:end
