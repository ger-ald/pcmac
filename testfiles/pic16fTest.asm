;pic16fxx test program (doesn't do anything usefull)

#list off
#if pass=2
#list on
#endif
#macros <pic16f.mac>

;<vectors>
RESETvect EQU 0000h
IRQvect   EQU 0004h
;</vectors>
;<registers>
PCLATH    EQU 0Ah
;</registers>
;<vars>
;</vars>

#list off
 ORG 2*RESETvect
	REPT 0 x 1024 times ;keeps the binary file clean
#if pass=2
#list on
#endif

 ORG 2*RESETvect
	NOP
	CLRW
	RETURN
	RETFIE
	SLEEP
	CLRWDT
LOOP	EQU $>>1;the goto arg is a word address (pic16fxx program memory has 14bit words, represented by 16bit instead of bytes)
	GOTO LOOP

 ORG 2*0F00h
LOOP2:
	NOP
	NOP
	NOP
	MOVLW (LOOP2>>1)>>8;the goto arg is a word address (pic16fxx program memory has 14bit words, represented by 16bit instead of bytes)
	MOVWF PCLATH
	GOTO LOOP2>>1
	GOTO ($>>1)-1
	GOTO 1h

LOOP3	EQU $>>1
	MOVLW LOOP3>>8
	MOVWF PCLATH
	GOTO LOOP3

	LBC "test123"
