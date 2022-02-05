@echo off
del "./windowsBuild/src/*"

mkdir "./windowsBuild/src"
cd "windowsBuild/src"

call "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvarsall.bat" x86 > nul
cl.exe %*
