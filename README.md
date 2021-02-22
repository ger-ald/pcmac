# pcmac
PCMAC is a two pass, symbolic cross assembler for the PC\XT\AT\PS2 computers. This assembler is a special one, because it is not dedicated to any processor. The user of PCMAC can decide what kind of assembly language to use, and what kind of code he wants to be generated. Therefore a very special macro language is supported in PCMAC that is powerful enough to define any assembly language like Z80 or MC68000. (Definition of a few cpu's with the z80 among them are included as samples.) The assembler supports many possibilities to maintain big assembly programs.

Some of the features:
* Special string and character expressions.
* Full C language like expressions.
* Powerful primitive commands to build up higher level assembly instruction.
* Directive and command line option driven listing.
* Symbol table listing.
* Fully compiled binary or linkable object code output chosen by the user.

The fact that you can also define your own cpu definition makes PCMAC ideal for homebuilt processors.

**PCMAC:**<br>
The source from PCMAC can be found in `assembler/src`.
Precompiled binaries for windows and linux can be found in the `bin` folder.

**Usage:**<br>
See `doc/usrguid.txt` to rtfm. But basically:
1. You put the .mac file for the cpu you want to target from `cpuDefinitions` next to your assembly file (.asm).
2. Call PCMAC with your assembly file as argument.<br>
    <sup>optionally add extra args like *-lnlist.txt* to generate a list.txt file</sup>
    ```bash
    > pcmac.exe yourasmsrc.asm
    ```
3. Out comes your binary (.bin).

Example of an assembly file:
```assembly
; these 4 lines are to make the generated list easier to read. (for the -ln option)
#list off
#if pass=2
	#list on
#endif

; include the cpu definition:
#macros <i8085.mac>

;<vectors>
RESETvect  EQU 0000h
TRAPvect   EQU 0024h
RST5_5vect EQU 002Ch
RST6_5vect EQU 0034h
RST7_5vect EQU 003Ch
;</vectors>

	ORG	RESETvect
	LXI SP,4800h
	SUB A ; A=0
LOOP:
	INR A
	ANI 0Fh
	JMP LOOP
```


PCMAC was written aeons ago (1987), and was released as shareware.
As it did not generate any revenue, it was formed to freeware as an XT compiled code.
Now it becomes 100% free ware, covered by the 'Apache V2.0' license. <sup>(I have written permission from the owner)</sup>
