rem assemble
"../../bin/pcmac.exe" -mn"zMC6303Listo.txt" -i"../../cpuDefinitions" -s zMC6303.asm -n"zMC6303o.bin"
"../../assembler/windowsBuild/pcmac.exe" -mn"zMC6303Listn.txt" -i"../../cpuDefinitions" -s zMC6303.asm -n"zMC6303n.bin"

rem make object
"../../bin/pcmac.exe" -mn"zMC6303Listo.txt" -i"../../cpuDefinitions" -s -o zMC6303.asm -n"zMC6303o.o"
"../../assembler/windowsBuild/pcmac.exe" -mn"zMC6303Listn.txt" -i"../../cpuDefinitions" -s -o zMC6303.asm -n"zMC6303n.o"

rem assemble
"../../bin/pcmac.exe" -mn"zExampleListo.txt" -i"../../cpuDefinitions" -s zExample.asm -n"zExampleo.bin"
"../../assembler/windowsBuild/pcmac.exe" -mn"zExampleListn.txt" -i"../../cpuDefinitions" -s zExample.asm -n"zExamplen.bin"

rem make object
"../../bin/pcmac.exe" -mn"zExampleListo.txt" -i"../../cpuDefinitions" -s -o zExample.asm -n"zExampleo.o"
"../../assembler/windowsBuild/pcmac.exe" -mn"zExampleListn.txt" -i"../../cpuDefinitions" -s -o zExample.asm -n"zExamplen.o"

pause
