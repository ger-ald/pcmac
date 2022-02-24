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
INDF		EQU 0h;(bank 0,1,2,3. or: 00h, 80h, 100h and 180h)
STATUS		EQU 3h ;(bank 0,1,2,3. or: 03h, 83h, 103h and 183h)
FSR			EQU 4h;(bank 0,1,2,3. or: 04h, 84h, 104h and 184h)
PCLATH		EQU 0Ah;(bank 0,1,2,3. or: 0Ah, 8Ah, 10Ah and 18Ah)
PORTA		EQU 5h ;(bank 0. or: 005h)
TRISA		EQU 5h ;(bank 1. or: 085h)
ADCON1		EQU 9h ;(bank 1. or: 09Fh)
;</registers>
;<status reg bits>
RP0			EQU 5
RP1			EQU 6
IRP			EQU 7
;</status reg bits>
;<vars>
VAR1		EQU 20h;(bank 0)
;</vars>


 ORG 2*RESETvect
	MOVLW ENTER>>8; set page address to where label is
	MOVWF PCLATH
	GOTO ENTER
	dd 0 ;keeps the binary file clean

ENTER 
	NOP
	CLRW
	RETURN
	RETFIE
	SLEEP
	CLRWDT

	; select bank 0
	BCF STATUS, RP0
	BCF STATUS, RP1
	; Initialize PORTA by
	; clearing output
	; data latches
	CLRF PORTA
	; Select Bank 1
	BSF STATUS, RP0
	; Configure all pins as digital inputs
	MOVLW 06h
	MOVWF ADCON1
	; Value used to initialize data direction 0b0 => output, 0b1 => input
	MOVLW 00h
	MOVWF TRISA
	
	;point indirect addressing to port a
	;BCF STATUS,IRP;bank 0/1
	;MOVLW PORTA
	;MOVWF FSR
	;CLRF INDF

	BCF STATUS, RP0; bank 0
	CLRF PCLATH; clear page address (address to jump to is in page 0)

	CLRF VAR1
LOOP	EQU $>>1;the goto arg is a word address (pic16fxx program memory has 14bit words, represented by 16bit instead of bytes)
	INCF VAR1,F
	MOVF VAR1,W
	MOVWF PORTA
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
