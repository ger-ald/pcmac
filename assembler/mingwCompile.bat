@echo off

REM when changing this, don't forget to check/change the gdb path in .vscode/launch.json
REM set COMPILERPATH="C:\mingw64\bin"
set COMPILERPATH="C:\mingw32\bin"

set CYGWINPATH="C:\cygwin64\bin"

SET PATH=%COMPILERPATH%;%CYGWINPATH%;%PATH%

make -f winmakefile %*
