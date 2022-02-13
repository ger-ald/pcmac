#include <hd6303.mac>

;------------------------------------------------------------
; Skip bytes (values undefined)
;------------------------------------------------------------
macro("DS\  *",NUMERIC)
#if #0<0
#error "Number of bytes to be skipped cannot be negative !"
#else
$       :=      doreloc($+#0)
#endif
endm

	;the next statement should result in one 0x0A byte
	db (5+5)

	;the next statement should result in one 0x32 byte
	db stridx(2,"0123")

; *************************************************************
; PROGRAM NAME: EXAMPLE
;
; $Revision: 1.13 $ Brian Smith $Date: 1998/06/08 14:38:00 $
;
; PURPOSE:
; This is not really a program; it is just a file showing some
; examples of the Assembly Language used by the HD6303 Assembler
;
; ASSEMBLER FORMAT:
;
;       Each line can have a LABEL, an INSTRUCTION & a COMMENT
;
;       LABELs start in column 1 with a letter and may contain
;       the letters A to Z, in upper or lower case, the digits
;       0 to 9, and the character '_'.
;
;       INSTRUCTIONs can use whitespace, for example
;       "LDA A #123" is equivalent to "LDA    A       #  123".
;
;       COMMENTs always start with a semi-colon and end with
;       the end-of-line, so each line of a multiple line
;       comment needs to start with a semi-colon.
;
;       Four different types of numbers can be used: Decimal,
;       Hexadecimal, Octal and Binary.
;
;       Decimal numbers consist of the digits 0 to 9, may be
;       negative, and may end in d' or 'D".
;       Examples: 123, -456, 255d, 321D
;
;       Hexadecimal numbers consist of the digits 0 to 9, and
;       A to F, and must start with a decimal digit (i.e. 0 to
;       9) and end with 'h' or 'H'.  Letters may be in upper or
;       lower case.
;       Examples: 123h, 7fH, 7FFh, 0FFh
;
;       Octal numbers consist of the digits 0 to 7, and must
;       end with 'o' or 'O' (that is a letter, not zero).
;       Examples: 123o, 77O (note that uppercase 'O' is not a
;       good choice of terminator!)
;
;       Binary numbers consist of the digits 0 and 1, and must
;       end with a 'b' or 'B'.
;       Examples: 1010b, 0101B, 111b
;
;       Data storage can be allocated using the DB, DW, DS, DSZ,
;       REPT and LBC directives:
;
;               DB     -  defines one or more bytes with
;                         preset values (e.g. DB  12h, 'A')
;
;               DW     -  defines one or more 16-bit words
;                         with preset values (e.g. DW 1234h)
;
;               DS     -  defines a block of undefined bytes,
;                         i.e. bytes which are not preset to
;                         any particular value (e.g. DS 5)
;
;               DSZ    -  defines one or more bytes, all preset
;                         to zero (e.g. DSZ 6)
;
;               REPT   -  defines one or more bytes, all preset
;                         to the same user-supplied value
;                         (e.g. REPT 255 x 3 times)
;
;               LBC    -  defines a leading byte-count string,
;                         i.e. first byte holds the length
;                         (e.g. LBC "Hello")
;
; *************************************************************
; My email address is:
;
;              bds@sugelan.co.uk
;
; *************************************************************

; To demonstrate local control of the handling of relative branch
; instructions in this program, remove the semicolon from the
; SIGNEDONLY line below to reject all relative branches with
; unsigned numeric offsets and remove the semicolon from the
; LABELSONLY line below to reject all relative branches which
; have numeric offsets (some sample instructions appear at the
; end of this example file to demonstrate the effect of these
; two variables):

;var SIGNEDONLY
;var LABELSONLY

; The SIGNEDONLY and LABELSONLY lines also appear in HITACHI.MAC
; and can be changed there to affect all ASM files, as an
; alternative to inserting the appropriate line in every ASM file.
; *************************************************************

        ORG     7000h           ; set the code start address

        RTS                     ; just in case this "program"
                                ; is actually executed!
;--------------------------------------------------------------
; Data Block Storage Allocation
;--------------------------------------------------------------

ZERO    DSZ  3                  ; three bytes, each set to zero
GAP     DS   6                  ; a gap of six undefined bytes
REPEAT  REPT 128 x 3 times      ; three bytes, each set to 128
        DSZ  10o                ; eight bytes, each set to zero
        DS   100B               ; four undefined bytes
        REPT 177o x 10b times   ; two bytes, each set to 127

;--------------------------------------------------------------
; Byte-based data storage (numeric)
;--------------------------------------------------------------

        DB  100b, 11o, 20, 0FFh ; four bytes with preset values

        DB  'A', '2', 'z'       ; can preset using ASCII chars
        DB  'A' - 64, '2' - '0' ; or expressions

BTSIZE  DB ENDBT - BTABLE       ; number of bytes in table
BTABLE  DB 1, 2, 3, 4, 5        ; five one-byte entries
ENDBT   EQU $                   ; $ = current location pointer

;--------------------------------------------------------------
; Byte-based data storage (strings)
;--------------------------------------------------------------

STR_0   LBC "Hello!"            ; first byte has string length

        DB 6, "Hello!"          ; equivalent to STR_0 example

STR_1   DB  "Error message",0   ; a null-terminated string

STR_2   DB  "Error message\0"   ; equivalent to STR_1 example

        ; Strings can include the following control chars
        ; \t = tab, \r = carriage return and \n = line-feed

STR_3   DB  "Hello, world!\r\n"
        DB  0                   ; a null terminator for STR_3

        ; Strings can also include escape characters to
        ; specify non-ASCII character codes (e.g. \242)

STR_4   LBC "Press \255 key"    ; inserts char with code 255

STR_5   DB 11, "Press ", 255, " key" ; equivalent to STR_4

;--------------------------------------------------------------
; Word-based data storage
;--------------------------------------------------------------

        DW  1234h, 5678h        ; two preset 16-bit words

        DW  ZERO                ; stores address of the label

        DW GAP, REPEAT          ; stores two 16-bit addresses

WTBYTES DB ENDWT - WTABLE       ; number of bytes in table
WTSIZE  DB +(ENDWT - WTABLE) / 2 ; number of entries in table
WTABLE  DW STR_0, STR_1, STR_2, STR_3, STR_4 ; five 16-bit entries
ENDWT   EQU $                   ; $ = current location pointer

DUMMY   TRAP                    ; Should never get here!

;--------------------------------------------------------------
; Relative branch instructions
;--------------------------------------------------------------

; The next 4 instructions are all accepted with the default settings
; (i.e. neither SIGNEDONLY nor LABELSONLY defined)
;
; If the variable SIGNEDONLY is defined then the last one of the four
; will be rejected
;
; If the variable LABELSONLY is defined then only the first one of the four
; will be accepted


        BCS DUMMY       ; Label accepted, if within range -128 to +127 bytes
        BNE -2          ; Signed numeric offset accepted in range -128 to +127
        BEQ +2          ; As above
        BPL 2           ; Unsigned positive offset accepted in range 0 to 127

; Of the next 4 instructions, the last one will be rejected if the
; variable SIGNEDONLY is defined

        BHI DUMMY       ; Label accepted, if within range -128 to +127 bytes
        BCC -3          ; Signed numeric offset accepted in range -128 to +127
        BVC +127        ; As above
        BLE 2           ; Will be rejected because no + or - sign present

; Of the next 4 instructions, only the first one will be accepted if the
; variable LABELSONLY is defined (state of SIGNEDONLY does not matter)

        BGT DUMMY       ; Label accepted, if within range -128 to +127 bytes
        BLT -3          ; Will be rejected because offset is numeric
        BMI +127        ; Will be rejected because offset is numeric
        BLS 2           ; Will be rejected because offset is numeric

; *************************************************************
;       End of EXAMPLE.ASM
; *************************************************************
