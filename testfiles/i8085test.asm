;i8085 test program (prints a human readable counter at DISPLAY_END)

; these 4 lines are to make the generated list easier to read. (for the -l option)
#list off
#if pass=2
#list on
#endif


; <file> is searched for in the include dir (if it is defined in
; the environment var or with option '-i"incdir"'. if not then it is the same as "file")
#macros <i8085.mac>


;<i8085 vectors>
RESETvect	EQU 0000h
TRAPvect	EQU 0024h
RST5_5vect	EQU 002Ch
RST6_5vect	EQU 0034h
RST7_5vect	EQU 003Ch
;</vectors>

RAMSTART	EQU 4000h
STACKPTR	EQU 4800h;0200h;4800h

;<vars>
VAR_COUNT	EQU 4000h
VAR_TMP1	EQU 4004h
DISPLAY_END	EQU 4020h
;</vars>


 ORG	RESETvect ;ORG is a macro so can't be on the first char in a line (so there must be a space or tab in front)
	JMP ENTER
 ORG	TRAPvect
	JMP TRAP_ISR
 ORG	RST7_5vect
	JMP RST7_5_ISR


;number constants (int8, int16, int32)
LIT_ONE:
	db 1
LIT_ZERO:
	dd 0

TRAP_ISR:
	PUSH PSW
	
	;isr code here

	RIM
	ANI 08h
	JZ TRAP_ISR_0
	POP PSW
	EI
	RET
TRAP_ISR_0:
	POP PSW
	RET


RST7_5_ISR:
	PUSH PSW

	;isr code here

	POP PSW
	EI
	RET



ENTER:
	LXI SP,STACKPTR

	LXI B,VAR_COUNT;dest (VAR_COUNT = 1)
	LXI H,LIT_ONE;source
	CALL MOV4BYTES

	MVI A,01011011b;enable RST7.5 reset sod[7-6] reset rst7.5[4]
	SIM
	EI

LOOP:
	LXI D,VAR_TMP1;dest
	LXI H,VAR_COUNT;source
	CALL DOUBLE_DABBLE

	;convert packed bcd to ascii:
	LXI D,VAR_TMP1
	LXI H,DISPLAY_END
	LDAX D
	MOV C,A
SOMELABEL:
	INX D
	LDAX D
	ANI 0Fh
	ADI 30h
	DCX H
	MOV M,A
	
	DCR C
	JNZ NOTLASTLOOP
	LDAX D
	ANI 0F0h
	JZ SKIPZERO
NOTLASTLOOP:
	INR C
	
	LDAX D
	RRC
	RRC
	RRC
	RRC
	ANI 0Fh
	ADI 30h
	DCX H
	MOV M,A
	DCR C
	JNZ SOMELABEL

SKIPZERO:

	;increment var and repeat:
	ANA A	;Cy=0
	LXI B,VAR_COUNT;dest
	LXI H,LIT_ONE;source
	CALL ADD4BYTES;VAR_COUNT++
	JMP LOOP




;[BC]=BCD
;converts n*8bit number to packed BCD
;input: C  = n
;       DE = destination[n]
;       HL = const source[n]
;output: LBC destination[n]
;        C = 0
;used: ABCDEHL
DOUBLE_DABBLE:
	MVI C,04h;amount of bytes

	MVI B,0h ;create *(ptr+n) (we have to go trough in reverse)
	DAD B ;HL+=n

	XCHG ;4
	INX H
	MVI M,00h;num=0
	DCX H
	MVI M,01h;LBC=1

;check for first nonzero byte
DOUBLE_DABBLE_BYTELOOP1:
	DCX D
	LDAX D
	ANA A ;do something to check if A==0
	JNZ DOUBLE_DABBLE_GOCHECKBITS
	DCR C
	JNZ DOUBLE_DABBLE_BYTELOOP1 ;next byte
	RET	;ret if number==0

;calculate howmany bits (C now becomes total bits to go instead of bytes)
DOUBLE_DABBLE_GOCHECKBITS:
	MOV A,C
	ADD A ;*2 (no carry consideration)
	ADD A ;*4
	ADD A ;*8  (max = 32bytes*8=256=0x100 which is 0x00 in 8 bits but will work)
	MOV C,A	;store bitcounter
	LDAX D
;check for first bit
DOUBLE_DABBLE_BITLOOP1:
	ADD A ;A<<=1 without going in a circle and without carry input
	JC DOUBLE_DABBLE_GOTOCONVERT	;continue to convert number
	DCR C
	JMP DOUBLE_DABBLE_BITLOOP1 ;next bit (the byte was nonzero so no catch for DCR)

DOUBLE_DABBLE_BITLOOP2:
	PUSH B
	PUSH H
;check and shift each byte (and then shift A and add carry)
;B=temp byte for ani on A
;C=byte count
;used: ABC..HL
	MOV C,M ;(byte counter)
	INX H
	ANA A ;Cy=0
DOUBLE_DABBLE_BYTELOOP3:
	PUSH PSW ;save the prev Cy
	MOV A,M
	MOV B,A
	ANI 0Fh;can be removed for unpacked bcd
	CPI 05h; = base/2 -> 10/2   (before shift so /2  (shift == *2))
	MOV A,B
	JM DOUBLE_DABBLE_SMALLERTHAN5;jmp if <5    (to not get packed bcd we sould add (256-10)/2 instead of (16-10)/2 (see below code comments))
	ADI 03h;remove for packed bcd
	;could add 80h then subtract (base/2)
;	ADI 7Bh;insert for unpacked bcd x= (0x100-base)/2 -> (256-10)/2   (the 0x100 is to make a byte overflow on the next shift (shift == *2 -> overflow/2))
	MOV B,A
DOUBLE_DABBLE_SMALLERTHAN5:
;	JMP DOUBLE_DABBLE_SMALLERTHAN50;insert for unpacked bcd
	ANI 0F0h
	CPI 50h
	MOV A,B
	JM DOUBLE_DABBLE_SMALLERTHAN50;jmp if <5
	ADI 30h
	MOV B,A
DOUBLE_DABBLE_SMALLERTHAN50:
	POP PSW ;restore Cy
	MOV A,B
	RAL
	MOV M,A

	INX H	;next byte
	DCR C	;decrement byte counter
	JNZ DOUBLE_DABBLE_BYTELOOP3 ;from RAL to here are no Cy modifying instructions (Cy must be saved during the check/adjust)
	JNC DOUBLE_DABBLE_NCAFTERCHECK1
	MVI M,01h ;add carry to next byte if loop is done but the last shift had a carry
DOUBLE_DABBLE_NCAFTERCHECK1:
	POP H
	JNC DOUBLE_DABBLE_NCAFTERCHECK2
	INR M ;also INR the LBC if loop is done and shift had carry
DOUBLE_DABBLE_NCAFTERCHECK2:
	POP B

	MOV A,B
	ADD A ;A<<=1 without going in a circle and without carry input
	JNC DOUBLE_DABBLE_NOADDCY
DOUBLE_DABBLE_GOTOCONVERT:
	INX H
	INR M ;INR if carry==1
	DCX H
DOUBLE_DABBLE_NOADDCY:
	MOV B,A

	MOV A,C
	DCR A
	ANI 07h
	JNZ DOUBLE_DABBLE_BYTELOOP2
	DCX D
	LDAX D
	MOV B,A
DOUBLE_DABBLE_BYTELOOP2:
	DCR C
	JNZ DOUBLE_DABBLE_BITLOOP2 ;next bit
	RET	;done!



;[BC]=[HL]
;copies nBytes to nBytes
;input: E  = n
;       BC = destination[n]
;       HL = const source[n]
;output: destination[n]
;        E = 0
;used: ABC.EHL
MOV4BYTES:
	MVI E,04h	;4bytes (byte counter)
MOVnBYTES:
	MOV A,M	;A = [HL]
	STAX B	;[BC] = A
	INX B	;next byte
	INX H	;next byte
	DCR E	;decrement byte counter
	JNZ MOVnBYTES
	RET

;[BC]=[BC]+[HL]
;adds nBytes+nBytes+Cy
;input: E  = n
;       BC = num1[n]
;       HL = const num2[n]
;       carry flag
;output: num1[n]
;        E = 0
;        carry flag
;used: ABC.EHL
ADD4BYTES:
	MVI E,04h	;4bytes (byte counter)
	;ANA A	;clear carry
ADDnBYTES:
	LDAX B	;A = [BC]
	ADC M	;add with carry
	STAX B	;[BC] = A
	INX B	;next byte
	INX H	;next byte
	DCR E	;decrement byte counter
	JNZ ADDnBYTES
	RET

;[HL]={Cy,[HL]}<<1
;shifts Cy<-nBytes<-Cy as in x[all of them]<<1
;input: D  = n
;       HL = num[n]
;       carry flag
;output: num1[n]
;        E = 0
;        carry flag
;used: A...EHL
SHIFTL4BYTES:
	MVI E,04h	;4bytes (byte counter)
SHIFTLnBYTES:
	MOV A,M
	RAL
	MOV M,A
	INX H	;next byte
	DCR E	;decrement byte counter
	JNZ SHIFTLnBYTES
	RET


;blocking delay (unless debug is defined)
;input: B  = amount of delay
;output: A = 0
;        B = 0
;used: AB.....
DELAY:
#ifndef debug
	MVI A,0E3h
DELAY1:
	DCR A
	CPI 00h
	JNZ DELAY1
	DCR B
	MOV A,B
	CPI 00h
	JNZ DELAY
#endif
	RET

;A to ascii
;converts A to ascii HEX (Aout=A[7-4] Bout=A[3-0])
;in:  A
;out: AB
;used: ABC....
TOHEX8b:
	MOV C,A;backup
	CALL TOHEX4b
	MOV B,A
	MOV A,C;restore
	RRC
	RRC
	RRC
	RRC
;no RET! (TOHEX4b must be next)

;converts A[3-0] to ascii HEX (Aout=A[3-0])
;in: A  out: A  used: A......
TOHEX4b:
	ANI 0Fh
	CPI 0Ah
	JM TOHEX4b1
	ADI 07h;A>=0xA
TOHEX4b1:
	ADI 30h
	RET


;the following function is excluded because usediv is not defined
;var usediv ;uncomment this line to include it
#ifdef usediv

;Inputs:
;     DEHL
;Outputs:
;     DEHL is the quotient
;     A is the remainder
;     BC = 10
;used: ABCDEHL
DEHL_Div_10:
	LXI B,0D0Ah ;         ld bc,$0D0A  3B
	XRA A ;A=0            xor a        1B
	XCHG ;                ex de,hl     1B
	DAD H ;               add hl,hl    1B
	RAL ;                 rla          1B
	DAD H ;               add hl,hl    1B
	RAL ;                 rla          1B
	DAD H ;               add hl,hl    1B
	RAL ;                 rla          1B
DEHL_Div_10_0:
	DAD H ;               add hl,hl    1B  -7
	RAL ;                 rla          1B
	CMP C ;               cp c         1B
	JC DEHL_Div_10_1 ;    jr c,$+4     2B
	SUB C ;               sub c        1B
	INR L ;               inc l        1B
DEHL_Div_10_1:
	DCR B ;             \ djnz $-7     2B  +4
	JNZ DEHL_Div_10_0 ; /
	XCHG ;                ex de,hl     1B
	MVI B,16 ;            ld b,16      2B
DEHL_Div_10_2:
	DAD H ;                            1B  -7
	RAL ;                 rla          1B
	CMP C;                cp c         1B
	JC DEHL_Div_10_3 ;    jr c,$+4     2B
	SUB C ;               sub c        1B
	INR L ;               inc l        1B
DEHL_Div_10_3:
	DCR B ;             \ djnz $-7     2B  +4
	JNZ DEHL_Div_10_2 ; /
	RET
#endif
