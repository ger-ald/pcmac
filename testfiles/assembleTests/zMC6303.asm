#include <hd6303.mac>

; ******************************************************************************
; PROGRAM NAME: MC6303
;
; $Revision: 1.15 $ by Brian Smith $Date: 1998/06/02 21:22:46 $
; Email address:       bds@sugelan.co.uk
;
; PURPOSE:
;       This is not really a program!  This file simply contains examples of all
;       the possible HD6303 instructions, including all the different addressing
;       modes.
;
;       It has been produced in order to test the output of the current set of
;       HD6303 Macros for the PCMAC Universal Macro Assembler.
;
;       It also serves as an example of the instructions accepted by the assembler.
;
;       The HD6303 Assembler accepts two different sets of mnemonics, known as the
;       "Standard" set and the "Alternate" set.  Both sets appear in this listing,
;       with appropriate comments.
;
;       Here are a few examples of the differences between the two sets:
;
;               Standard                        Alternate
;               --------                        ---------
;               ADC A #123                      ADCA #123
;               ADC A OneByteAddr               ADCA OneByteAddr
;               ADC A Index,X                   ADCA Index,X
;               ADC A TwoByteAddr               ADCA TwoByteAddr
;               ASL A                           ASLA
;               ASL B                           ASLB
;               NEG A                           NEGA
;
;       The "Standard" set is the set I prefer.  However some people prefer the
;       other set so it is supported by the HD6303 Assembler (I see no reason to
;       force users to use "my" set!)  The assembler does not need to be configured
;       to use a particular set so assembly language programs can even use a mixture
;       of the two sets (as shown in this "program").
;
;       Immediate data values are indicated by the prefix "#" or the prefix "%"
;       e.g. the "ADC A #123" instruction shown above can also be written
;       as "ADC A %123" (and "ADCA #123" can be written as "ADCA %123").
;
;       The instructions are listed in alphabetic order, for convenience, and a
;       very brief description is given for each instruction.
;
;       The PCMAC documentation contains a full description of the format and
;       structure of source files.  See the EXAMPLE.ASM file for a few brief
;       examples of PCMAC directives etc.
;
; ******************************************************************************
; ******************************************************************************
;
;                   HD6303      Instruction      Set      Examples
;
; ******************************************************************************
; ******************************************************************************

OneByteAddr     EQU     56h     ; 8-bit hex value, used where 8-bit addr needed
TwoByteAddr     EQU     1234h   ; 16-bit hex value, used where 16-bit addr needed
Index           EQU     100     ; Decimal 100, used for offset in indexed instns

        ORG     7000h

START   ABA                     ; Accumulator A (Acc A) = Acc A + Acc B

        ABX                     ; Index Register = Acc B + Index Register

; ------------------------------------------------------------------------------
; Standard:
        ADC A #123              ; Acc A = Acc A + immediate data + carry
        ADC A OneByteAddr       ; Acc A = Acc A + direct addr contents + carry
        ADC A Index,X           ; Acc A = Acc A + indexed data contents + carry
        ADC A TwoByteAddr       ; Acc A = Acc A + extended addr contents + carry
; Alternate:
        ADCA #123               ; Acc A = Acc A + immediate data + carry
        ADCA OneByteAddr        ; Acc A = Acc A + direct addr contents + carry
        ADCA Index,X            ; Acc A = Acc A + indexed data contents + carry
        ADCA TwoByteAddr        ; Acc A = Acc A + extended addr contents + carry
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        ADC B %123              ; Acc B = Acc B + immediate data + carry
        ADC B OneByteAddr       ; Acc B = Acc B + direct addr contents + carry
        ADC B Index,X           ; Acc B = Acc B + indexed data contents + carry
        ADC B TwoByteAddr       ; Acc B = Acc B + extended addr contents + carry
; Alternate:
        ADCB %123               ; Acc B = Acc B + immediate data + carry
        ADCB OneByteAddr        ; Acc B = Acc B + direct addr contents + carry
        ADCB Index,X            ; Acc B = Acc B + indexed data contents + carry
        ADCB TwoByteAddr        ; Acc B = Acc B + extended addr contents + carry
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        ADD A %123              ; Acc A = Acc A + immediate data
        ADD A OneByteAddr       ; Acc A = Acc A + direct addr contents
        ADD A Index,X           ; Acc A = Acc A + indexed data contents
        ADD A TwoByteAddr       ; Acc A = Acc A + extended addr contents
; Alternate:
        ADDA %123               ; Acc A = Acc A + immediate data
        ADDA OneByteAddr        ; Acc A = Acc A + direct addr contents
        ADDA Index,X            ; Acc A = Acc A + indexed data contents
        ADDA TwoByteAddr        ; Acc A = Acc A + extended addr contents
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        ADD B #123              ; Acc B = Acc B + immediate data
        ADD B OneByteAddr       ; Acc B = Acc B + direct addr contents
        ADD B Index,X           ; Acc B = Acc B + indexed data contents
        ADD B TwoByteAddr       ; Acc B = Acc B + extended addr contents
; Alternate:
        ADDB #123               ; Acc B = Acc B + immediate data
        ADDB OneByteAddr        ; Acc B = Acc B + direct addr contents
        ADDB Index,X            ; Acc B = Acc B + indexed data contents
        ADDB TwoByteAddr        ; Acc B = Acc B + extended addr contents
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        ADD D #4567h            ; Acc D = Acc D + immediate data
        ADD D OneByteAddr       ; Acc D = Acc D + direct addr contents
        ADD D Index,X           ; Acc D = Acc D + indexed data contents
        ADD D TwoByteAddr       ; Acc D = Acc D + extended addr contents
; Alternate:
        ADDD %4567h             ; Acc D = Acc D + immediate data
        ADDD OneByteAddr        ; Acc D = Acc D + direct addr contents
        ADDD Index,X            ; Acc D = Acc D + indexed data contents
        ADDD TwoByteAddr        ; Acc D = Acc D + extended addr contents
; ------------------------------------------------------------------------------

        AIM #123,OneByteAddr    ; Direct addr contents = contents AND immed data
        AIM %123,Index,X        ; Indexed data contents = contents AND immed data

; ------------------------------------------------------------------------------
; Standard:
        AND A #123              ; Acc A = Acc A AND immediate data
        AND A OneByteAddr       ; Acc A = Acc A AND direct addr contents
        AND A Index,X           ; Acc A = Acc A AND indexed data contents
        AND A TwoByteAddr       ; Acc A = Acc A AND extended addr contents
; Alternate:
        ANDA #123               ; Acc A = Acc A AND immediate data
        ANDA OneByteAddr        ; Acc A = Acc A AND direct addr contents
        ANDA Index,X            ; Acc A = Acc A AND indexed data contents
        ANDA TwoByteAddr        ; Acc A = Acc A AND extended addr contents
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        AND B #123              ; Acc B = Acc B AND immediate data
        AND B OneByteAddr       ; Acc B = Acc B AND direct addr contents
        AND B Index,X           ; Acc B = Acc B AND indexed data contents
        AND B TwoByteAddr       ; Acc B = Acc B AND extended addr contents
; Alternate:
        ANDB #123               ; Acc B = Acc B AND immediate data
        ANDB OneByteAddr        ; Acc B = Acc B AND direct addr contents
        ANDB Index,X            ; Acc B = Acc B AND indexed data contents
        ANDB TwoByteAddr        ; Acc B = Acc B AND extended addr contents
; ------------------------------------------------------------------------------

        ; ASL = Arithmetic Shift Left, 1 place

; ------------------------------------------------------------------------------
; Standard:
        ASL A                   ; Acc A = Acc A ASL
        ASL B                   ; Acc B = Acc B ASL
; Alternate:
        ASLA                    ; Acc A = Acc A ASL
        ASLB                    ; Acc B = Acc B ASL
; ------------------------------------------------------------------------------

        ASL Index,X             ; Indexed data contents = contents ASL
        ASL TwoByteAddr         ; Extended addr contents = contents ASL

        ASLD                    ; Acc D = Acc D ASL

        ; ASR = Arithmetic Shift Right, 1 place

; ------------------------------------------------------------------------------
; Standard:
        ASR A                   ; Acc A = Acc A ASR
        ASR B                   ; Acc B = Acc B ASR
; Alternate:
        ASRA                    ; Acc A = Acc A ASR
        ASRB                    ; Acc B = Acc B ASR
; ------------------------------------------------------------------------------

        ASR Index,X             ; Indexed data contents = contents ASR
        ASR TwoByteAddr         ; Extended addr contents = contents ASR

        BCC Label_A             ; Branch if carry clear
        BCC +3                  ; This branch is relative to NEXT instruction
                                ; i.e. branch is to 3 bytes beyond "Label_0"

Label_0 BCLR 0,OneByteAddr      ; Clear bit 0 of direct addr contents
        BCLR 1,OneByteAddr      ; Clear bit 1 of direct addr contents
        BCLR 2,OneByteAddr      ; Clear bit 2 of direct addr contents
        BCLR 3,OneByteAddr      ; Clear bit 3 of direct addr contents
        BCLR 4,OneByteAddr      ; Clear bit 4 of direct addr contents
        BCLR 5,OneByteAddr      ; Clear bit 5 of direct addr contents
        BCLR 6,OneByteAddr      ; Clear bit 6 of direct addr contents
        BCLR 7,OneByteAddr      ; Clear bit 7 of direct addr contents

Label_A BCLR 0,Index,X          ; Clear bit 0 of indexed data contents
        BCLR 1,Index,X          ; Clear bit 1 of indexed data contents
        BCLR 2,Index,X          ; Clear bit 2 of indexed data contents
        BCLR 3,Index,X          ; Clear bit 3 of indexed data contents
        BCLR 4,Index,X          ; Clear bit 4 of indexed data contents
        BCLR 5,Index,X          ; Clear bit 5 of indexed data contents
        BCLR 6,Index,X          ; Clear bit 6 of indexed data contents
        BCLR 7,Index,X          ; Clear bit 7 of indexed data contents

        BCS Label_A             ; Branch if carry set
        BCS -128                ; This branch is relative to next instruction
                                ; i.e. branch is to 128 bytes before "Label_1"

Label_1 BEQ Label_A             ; Branch if equal to zero
        BEQ -127                ; This branch is relative to next instruction

        BGE Label_A             ; Branch if greater than or equal to zero
        BGE -64                ; This branch is relative to next instruction

        BGT Label_A             ; Branch if greater than zero
        BGT +123                ; This branch is relative to next instruction

        BHI Label_A             ; Branch if higher
        BHI 127                 ; This branch is relative to next instruction

; ------------------------------------------------------------------------------
; Standard:
        BIT A #123              ; Bit test, Acc A AND immediate data
        BIT A OneByteAddr       ; Bit test, Acc A AND direct addr contents
        BIT A Index,X           ; Bit test, Acc A AND indexed data contents
        BIT A TwoByteAddr       ; Bit test, Acc A AND extended addr contents
; Alternate:
        BITA #123               ; Bit test, Acc A AND immediate data
        BITA OneByteAddr        ; Bit test, Acc A AND direct addr contents
        BITA Index,X            ; Bit test, Acc A AND indexed data contents
        BITA TwoByteAddr        ; Bit test, Acc A AND extended addr contents
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        BIT B #123              ; Bit test, Acc B AND immediate data
        BIT B OneByteAddr       ; Bit test, Acc B AND direct addr contents
        BIT B Index,X           ; Bit test, Acc B AND indexed data contents
        BIT B TwoByteAddr       ; Bit test, Acc B AND extended addr contents
; Alternate:
        BITB #123               ; Bit test, Acc B AND immediate data
        BITB OneByteAddr        ; Bit test, Acc B AND direct addr contents
        BITB Index,X            ; Bit test, Acc B AND indexed data contents
        BITB TwoByteAddr        ; Bit test, Acc B AND extended addr contents
; ------------------------------------------------------------------------------

        BLE Label_A             ; Branch if less than or equal to zero
        BLE 123                 ; This branch is relative to next instruction
                                ; i.e. branch is to 123 bytes beyond "Label_2"

Label_2 BLS Label_A             ; Branch if lower or same as
        BLS 0                   ; This branch is relative to next instruction

        BLT Label_A             ; Branch if less than zero
        BLT -111B               ; This branch is relative to next instruction

        BMI Label_A             ; Branch if minus
        BMI -100                ; This branch is relative to next instruction

        BNE Label_A             ; Branch if not equal to zero
        BNE 100                 ; This branch is relative to next instruction

        BPL Label_A             ; Branch if plus
        BPL +10h                ; This branch is relative to next instruction

        BRA Label_A             ; Branch always
        BRA -10h                ; This branch is relative to next instruction

        BRN Label_A             ; Branch never
        BRN -16                 ; This branch is relative to next instruction
                                ; i.e. branch is to 16 bytes before "Label_3"

Label_3 BSET 0,OneByteAddr      ; Set bit 0 of direct addr contents
        BSET 1,OneByteAddr      ; Set bit 1 of direct addr contents
        BSET 2,OneByteAddr      ; Set bit 2 of direct addr contents
        BSET 3,OneByteAddr      ; Set bit 3 of direct addr contents
        BSET 4,OneByteAddr      ; Set bit 4 of direct addr contents
        BSET 5,OneByteAddr      ; Set bit 5 of direct addr contents
        BSET 6,OneByteAddr      ; Set bit 6 of direct addr contents
        BSET 7,OneByteAddr      ; Set bit 7 of direct addr contents

        BSET 0,Index,X          ; Set bit 0 of indexed data contents
        BSET 1,Index,X          ; Set bit 1 of indexed data contents
        BSET 2,Index,X          ; Set bit 2 of indexed data contents
        BSET 3,Index,X          ; Set bit 3 of indexed data contents
        BSET 4,Index,X          ; Set bit 4 of indexed data contents
        BSET 5,Index,X          ; Set bit 5 of indexed data contents
        BSET 6,Index,X          ; Set bit 6 of indexed data contents
        BSET 7,Index,X          ; Set bit 7 of indexed data contents

        BSR Label_B             ; Branch to subroutine
        BSR 123                 ; This branch is relative to next instruction
                                ; i.e. branch is to 123 bytes beyond "Label_4"

Label_4 BTGL 0,OneByteAddr      ; Invert bit 0 of direct addr contents
        BTGL 1,OneByteAddr      ; Invert bit 1 of direct addr contents
        BTGL 2,OneByteAddr      ; Invert bit 2 of direct addr contents
        BTGL 3,OneByteAddr      ; Invert bit 3 of direct addr contents
        BTGL 4,OneByteAddr      ; Invert bit 4 of direct addr contents
        BTGL 5,OneByteAddr      ; Invert bit 5 of direct addr contents
        BTGL 6,OneByteAddr      ; Invert bit 6 of direct addr contents
        BTGL 7,OneByteAddr      ; Invert bit 7 of direct addr contents

        BTGL 0,Index,X          ; Invert bit 0 of indexed data contents
        BTGL 1,Index,X          ; Invert bit 1 of indexed data contents
        BTGL 2,Index,X          ; Invert bit 2 of indexed data contents
        BTGL 3,Index,X          ; Invert bit 3 of indexed data contents
        BTGL 4,Index,X          ; Invert bit 4 of indexed data contents
        BTGL 5,Index,X          ; Invert bit 5 of indexed data contents
        BTGL 6,Index,X          ; Invert bit 6 of indexed data contents
        BTGL 7,Index,X          ; Invert bit 7 of indexed data contents

        BTST 0,OneByteAddr      ; Test bit 0 of direct addr contents
        BTST 1,OneByteAddr      ; Test bit 1 of direct addr contents
        BTST 2,OneByteAddr      ; Test bit 2 of direct addr contents
        BTST 3,OneByteAddr      ; Test bit 3 of direct addr contents
        BTST 4,OneByteAddr      ; Test bit 4 of direct addr contents
        BTST 5,OneByteAddr      ; Test bit 5 of direct addr contents
        BTST 6,OneByteAddr      ; Test bit 6 of direct addr contents
        BTST 7,OneByteAddr      ; Test bit 7 of direct addr contents

        BTST 0,Index,X          ; Test bit 0 of indexed data contents
        BTST 1,Index,X          ; Test bit 1 of indexed data contents
        BTST 2,Index,X          ; Test bit 2 of indexed data contents
        BTST 3,Index,X          ; Test bit 3 of indexed data contents
        BTST 4,Index,X          ; Test bit 4 of indexed data contents
        BTST 5,Index,X          ; Test bit 5 of indexed data contents
        BTST 6,Index,X          ; Test bit 6 of indexed data contents
        BTST 7,Index,X          ; Test bit 7 of indexed data contents

        BVC Label_B             ; Branch if overflow clear
        BVC +2                  ; This branch is relative to next instruction
                                ; i.e. branch is to 2 bytes beyond "Label_5"

Label_5 BVS Label_B             ; Branch if overflow set
        BVS -6                  ; This branch is relative to next instruction

        CBA                     ; Compare accumulators A and B

        CLC                     ; Clear carry bit

Label_B CLI                     ; Clear interrupt mask

; ------------------------------------------------------------------------------
; Standard:
        CLR A                   ; Clear Acc A
        CLR B                   ; Clear Acc B
; Alternate:
        CLRA                    ; Clear Acc A
        CLRB                    ; Clear Acc B
; ------------------------------------------------------------------------------

        CLR Index,X             ; Clear indexed data contents
        CLR TwoByteAddr         ; Clear extended addr contents

        CLV                     ; Clear overflow bit

; ------------------------------------------------------------------------------
; Standard:
        CMP A #123              ; Compare Acc A with immediate data
        CMP A OneByteAddr       ; Compare Acc A with direct addr contents
        CMP A Index,X           ; Compare Acc A with indexed data contents
        CMP A TwoByteAddr       ; Compare Acc A with extended addr contents
; Alternate:
        CMPA #123               ; Compare Acc A with immediate data
        CMPA OneByteAddr        ; Compare Acc A with direct addr contents
        CMPA Index,X            ; Compare Acc A with indexed data contents
        CMPA TwoByteAddr        ; Compare Acc A with extended addr contents
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        CMP B #123              ; Compare Acc B with immediate data
        CMP B OneByteAddr       ; Compare Acc B with direct addr contents
        CMP B Index,X           ; Compare Acc B with indexed data contents
        CMP B TwoByteAddr       ; Compare Acc B with extended addr contents
; Alternate:
        CMPB #123               ; Compare Acc B with immediate data
        CMPB OneByteAddr        ; Compare Acc B with direct addr contents
        CMPB Index,X            ; Compare Acc B with indexed data contents
        CMPB TwoByteAddr        ; Compare Acc B with extended addr contents
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        COM A                   ; Acc A = Ones Complement of Acc A
        COM B                   ; Acc B = Ones Complement of Acc B
; Alternate:
        COMA                    ; Acc A = Ones Complement of Acc A
        COMB                    ; Acc B = Ones Complement of Acc B
; ------------------------------------------------------------------------------

        COM Index,X             ; Indexed data contents = Ones Complement
        COM TwoByteAddr         ; Extended addr contents = Ones Complement

        CPX #1234h              ; Compare Index register with immediate data
        CPX OneByteAddr         ; Compare Index register with direct addr contents
        CPX Index,X             ; Compare Index register with indexed data contents
        CPX TwoByteAddr         ; Compare Index register with extended addr content

        DAA                     ; Decimal Adjust Acc A contents

; ------------------------------------------------------------------------------
; Standard:
        DEC A                   ; Decrement Acc A contents by 1
        DEC B                   ; Decrement Acc B contents by 1
; Alternate:
        DECA                    ; Decrement Acc A contents by 1
        DECB                    ; Decrement Acc B contents by 1
; ------------------------------------------------------------------------------

        DEC Index,X             ; Decrement indexed data contents by 1
        DEC TwoByteAddr         ; Decrement extended addr contents by 1

        DES                     ; Decrement stack pointer by 1

        DEX                     ; Decrement index register by 1

        EIM #123,OneByteAddr    ; Direct addr contents = contents EOR immed data
        EIM #123,Index,X        ; Indexed data contents = contents EOR immed data

; ------------------------------------------------------------------------------
; Standard:
        EOR A #123              ; Acc A = Acc A EOR immediate data (exclusive OR)
        EOR A OneByteAddr       ; Acc A = Acc A EOR direct addr contents
        EOR A Index,X           ; Acc A = Acc A EOR indexed data contents
        EOR A TwoByteAddr       ; Acc A = Acc A EOR extended addr contents
; Alternate:
        EORA #123               ; Acc A = Acc A EOR immediate data (exclusive OR)
        EORA OneByteAddr        ; Acc A = Acc A EOR direct addr contents
        EORA Index,X            ; Acc A = Acc A EOR indexed data contents
        EORA TwoByteAddr        ; Acc A = Acc A EOR extended addr contents
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        EOR B #123              ; Acc B = Acc B EOR immediate data (exclusive OR)
        EOR B OneByteAddr       ; Acc B = Acc B EOR direct addr contents
        EOR B Index,X           ; Acc B = Acc B EOR indexed data contents
        EOR B TwoByteAddr       ; Acc B = Acc B EOR extended addr contents
; Alternate:
        EORB #123               ; Acc B = Acc B EOR immediate data (exclusive OR)
        EORB OneByteAddr        ; Acc B = Acc B EOR direct addr contents
        EORB Index,X            ; Acc B = Acc B EOR indexed data contents
        EORB TwoByteAddr        ; Acc B = Acc B EOR extended addr contents
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        INC A                   ; Increment Acc A contents by 1
        INC B                   ; Increment Acc B contents by 1
; Alternate:
        INCA                    ; Increment Acc A contents by 1
        INCB                    ; Increment Acc B contents by 1
; ------------------------------------------------------------------------------

        INC Index,X             ; Increment indexed data contents by 1
        INC TwoByteAddr         ; Increment extended addr contents by 1

        INS                     ; Increment stack pointer by 1

        INX                     ; Increment index register by 1

        JMP Index,X             ; Jump to indexed addr
        JMP TwoByteAddr         ; Jump to extended addr

        JSR OneByteAddr         ; Jump to subroutine (direct addr)
        JSR Index,X             ; Jump to subroutine (indexed addr)
        JSR TwoByteAddr         ; Jump to subroutine (extended addr)

; ------------------------------------------------------------------------------
; Standard:
        LDA A #'A'              ; Acc A = immediate data
        LDA A OneByteAddr       ; Acc A = direct addr contents
        LDA A Index,X           ; Acc A = indexed data contents
        LDA A TwoByteAddr       ; Acc A = extended addr contents
; Alternate:
        LDAA #'A'               ; Acc A = immediate data
        LDAA OneByteAddr        ; Acc A = direct addr contents
        LDAA Index,X            ; Acc A = indexed data contents
        LDAA TwoByteAddr        ; Acc A = extended addr contents
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        LDA B #'a'              ; Acc B = immediate data
        LDA B OneByteAddr       ; Acc B = direct addr contents
        LDA B Index,X           ; Acc B = indexed data contents
        LDA B TwoByteAddr       ; Acc B = extended addr contents
; Alternate:
        LDAB #'a'               ; Acc B = immediate data
        LDAB OneByteAddr        ; Acc B = direct addr contents
        LDAB Index,X            ; Acc B = indexed data contents
        LDAB TwoByteAddr        ; Acc B = extended addr contents
; ------------------------------------------------------------------------------

        LDD #Label_B - Label_A  ; Acc D = immediate data
        LDD OneByteAddr         ; Acc D = direct addr contents
        LDD Index,X             ; Acc D = indexed data contents
        LDD TwoByteAddr         ; Acc D = extended addr contents

        LDS #Label_A - 113      ; Stack pointer = immediate data
        LDS OneByteAddr         ; Stack pointer = direct addr contents
        LDS Index,X             ; Stack pointer = indexed data contents
        LDS TwoByteAddr         ; Stack pointer = extended addr contents

        LDX #8*16+4             ; Index register = immediate data
        LDX OneByteAddr         ; Index register = direct addr contents
        LDX Index,X             ; Index register = indexed data contents
        LDX TwoByteAddr         ; Index register = extended addr contents

        ; LSR = Logical Shift Right, 1 place

; ------------------------------------------------------------------------------
; Standard:
        LSR A                   ; Acc A = Acc A LSR
        LSR B                   ; Acc B = Acc B LSR
; Alternate:
        LSRA                    ; Acc A = Acc A LSR
        LSRB                    ; Acc B = Acc B LSR
; ------------------------------------------------------------------------------

        LSR Index,X             ; Indexed data contents = contents LSR
        LSR TwoByteAddr         ; Extended addr contents = contents LSR

        LSRD                    ; Acc D = Acc D LSR

        MUL                     ; Acc D = Acc A * Acc B, unsigned multiply

; ------------------------------------------------------------------------------
; Standard:
        NEG A                   ; Acc A = Twos complement of Acc A (i.e. negate it)
        NEG B                   ; Acc B = Twos complement of Acc B
; Alternate:
        NEGA                    ; Acc A = Twos complement of Acc A (i.e. negate it)
        NEGB                    ; Acc B = Twos complement of Acc B
; ------------------------------------------------------------------------------

        NEG Index,X             ; Indexed data contents = Twos complement
        NEG TwoByteAddr         ; Extended addr contents = Twos complement

        NOP                     ; No operation

        OIM #123,OneByteAddr    ; Direct addr contents = contents OR immed data
        OIM #123,Index,X        ; Indexed data contents = contents OR immed data

; ------------------------------------------------------------------------------
; Standard:
        ORA A # 8*16+4          ; Acc A = Acc A OR immediate data
        ORA A OneByteAddr       ; Acc A = Acc A OR direct addr contents
        ORA A Index,X           ; Acc A = Acc A OR indexed data contents
        ORA A TwoByteAddr       ; Acc A = Acc A OR extended addr contents
; Alternate:
        ORAA # 8*16+4           ; Acc A = Acc A OR immediate data
        ORAA OneByteAddr        ; Acc A = Acc A OR direct addr contents
        ORAA Index,X            ; Acc A = Acc A OR indexed data contents
        ORAA TwoByteAddr        ; Acc A = Acc A OR extended addr contents
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        ORA B #123              ; Acc B = Acc B OR immediate data
        ORA B OneByteAddr       ; Acc B = Acc B OR direct addr contents
        ORA B Index,X           ; Acc B = Acc B OR indexed data contents
        ORA B TwoByteAddr       ; Acc B = Acc B OR extended addr contents
; Alternate:
        ORAB %123               ; Acc B = Acc B OR immediate data
        ORAB OneByteAddr        ; Acc B = Acc B OR direct addr contents
        ORAB Index,X            ; Acc B = Acc B OR indexed data contents
        ORAB TwoByteAddr        ; Acc B = Acc B OR extended addr contents
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        PSH A                   ; Push Acc A onto the stack
        PSH B                   ; Push Acc B onto the stack
; Alternate:
        PSHA                    ; Push Acc A onto the stack
        PSHB                    ; Push Acc B onto the stack
; ------------------------------------------------------------------------------

        PSHX                    ; Push index register onto the stack

; ------------------------------------------------------------------------------
; Standard:
        PUL A                   ; Pull Acc A off the stack
        PUL B                   ; Pull Acc B off the stack
; Alternate:
        PULA                    ; Pull Acc A off the stack
        PULB                    ; Pull Acc B off the stack
; ------------------------------------------------------------------------------

        PULX                    ; Pull index register off the stack

        ; ROL = Rotate Left, 1 place

; ------------------------------------------------------------------------------
; Standard:
        ROL A                   ; Acc A = Acc A ROL, using carry
        ROL B                   ; Acc B = Acc B ROL, using carry
; Alternate:
        ROLA                    ; Acc A = Acc A ROL, using carry
        ROLB                    ; Acc B = Acc B ROL, using carry
; ------------------------------------------------------------------------------

        ROL Index,X             ; Indexed data contents = contents ROL + carry
        ROL TwoByteAddr         ; Extended addr contents = contents ROL + carry

        ; ROR = Rotate Right, 1 place

; ------------------------------------------------------------------------------
; Standard:
        ROR A                   ; Acc A = Acc A ROR, using carry
        ROR B                   ; Acc B = Acc B ROR, using carry
; Alternate:
        RORA                    ; Acc A = Acc A ROR, using carry
        RORB                    ; Acc B = Acc B ROR, using carry
; ------------------------------------------------------------------------------

        ROR Index,X             ; Indexed data contents = contents ROR + carry
        ROR TwoByteAddr         ; Extended addr contents = contents ROR + carry

        RTI                     ; Return from interrupt

        RTS                     ; Return from subroutine

        SBA                     ; Acc A = Acc A - Acc B

; ------------------------------------------------------------------------------
; Standard:
        SBC A #123              ; Acc A = Acc A - immediate data + carry
        SBC A OneByteAddr       ; Acc A = Acc A - direct addr contents + carry
        SBC A Index,X           ; Acc A = Acc A - indexed data contents + carry
        SBC A TwoByteAddr       ; Acc A = Acc A - extended addr contents + carry
; Alternate:
        SBCA #123               ; Acc A = Acc A - immediate data + carry
        SBCA OneByteAddr        ; Acc A = Acc A - direct addr contents + carry
        SBCA Index,X            ; Acc A = Acc A - indexed data contents + carry
        SBCA TwoByteAddr        ; Acc A = Acc A - extended addr contents + carry
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        SBC B #123              ; Acc B = Acc B - immediate data + carry
        SBC B OneByteAddr       ; Acc B = Acc B - direct addr contents + carry
        SBC B Index,X           ; Acc B = Acc B - indexed data contents + carry
        SBC B TwoByteAddr       ; Acc B = Acc B - extended addr contents + carry
; Alternate:
        SBCB #123               ; Acc B = Acc B - immediate data + carry
        SBCB OneByteAddr        ; Acc B = Acc B - direct addr contents + carry
        SBCB Index,X            ; Acc B = Acc B - indexed data contents + carry
        SBCB TwoByteAddr        ; Acc B = Acc B - extended addr contents + carry
; ------------------------------------------------------------------------------

        SEC                     ; Set carry bit

        SEI                     ; Set interrupt mask

        SEV                     ; Set overflow bit

        SLP                     ; Sleep

; ------------------------------------------------------------------------------
; Standard:
        STA A OneByteAddr       ; Direct addr contents = Acc A
        STA A Index,X           ; Indexed data contents = Acc A
        STA A TwoByteAddr       ; Extended addr contents = Acc A
; Alternate:
        STAA OneByteAddr        ; Direct addr contents = Acc A
        STAA Index,X            ; Indexed data contents = Acc A
        STAA TwoByteAddr        ; Extended addr contents = Acc A
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        STA B OneByteAddr       ; Direct addr contents = Acc B
        STA B Index,X           ; Indexed data contents = Acc B
        STA B TwoByteAddr       ; Extended addr contents = Acc B
; Alternate:
        STAB OneByteAddr        ; Direct addr contents = Acc B
        STAB Index,X            ; Indexed data contents = Acc B
        STAB TwoByteAddr        ; Extended addr contents = Acc B
; ------------------------------------------------------------------------------

        STD OneByteAddr         ; Direct addr contents = Acc D
        STD Index,X             ; Indexed data contents = Acc D
        STD TwoByteAddr         ; Extended addr contents = Acc D

        STS OneByteAddr         ; Direct addr contents = Stack pointer
        STS Index,X             ; Indexed data contents = Stack pointer
        STS TwoByteAddr         ; Extended addr contents = Stack pointer

        STX OneByteAddr         ; Direct addr contents = Index register
        STX Index,X             ; Indexed data contents = Index register
        STX TwoByteAddr         ; Extended addr contents = Index register

; ------------------------------------------------------------------------------
; Standard:
        SUB A #123              ; Acc A = Acc A - immediate data
        SUB A OneByteAddr       ; Acc A = Acc A - direct addr contents
        SUB A Index,X           ; Acc A = Acc A - indexed data contents
        SUB A TwoByteAddr       ; Acc A = Acc A - extended addr contents
; Alternate:
        SUBA #123               ; Acc A = Acc A - immediate data
        SUBA OneByteAddr        ; Acc A = Acc A - direct addr contents
        SUBA Index,X            ; Acc A = Acc A - indexed data contents
        SUBA TwoByteAddr        ; Acc A = Acc A - extended addr contents
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        SUB B #123              ; Acc B = Acc B - immediate data
        SUB B OneByteAddr       ; Acc B = Acc B - direct addr contents
        SUB B Index,X           ; Acc B = Acc B - indexed data contents
        SUB B TwoByteAddr       ; Acc B = Acc B - extended addr contents
; Alternate:
        SUBB #123               ; Acc B = Acc B - immediate data
        SUBB OneByteAddr        ; Acc B = Acc B - direct addr contents
        SUBB Index,X            ; Acc B = Acc B - indexed data contents
        SUBB TwoByteAddr        ; Acc B = Acc B - extended addr contents
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Standard:
        SUB D #1234h            ; Acc D = Acc D - immediate data
        SUB D OneByteAddr       ; Acc D = Acc D - direct addr contents
        SUB D Index,X           ; Acc D = Acc D - indexed data contents
        SUB D TwoByteAddr       ; Acc D = Acc D - extended addr contents
; Alternate:
        SUBD %1234h             ; Acc D = Acc D - immediate data
        SUBD OneByteAddr        ; Acc D = Acc D - direct addr contents
        SUBD Index,X            ; Acc D = Acc D - indexed data contents
        SUBD TwoByteAddr        ; Acc D = Acc D - extended addr contents
; ------------------------------------------------------------------------------

        SWI #123                ; Software interrupt

        TAB                     ; Transfer accumulators, Acc B = Acc A

        TAP                     ; Condition Code Register = bits 0 - 5 of Acc A

        TBA                     ; Transfer accumulators, Acc A = Acc B

        TIM #123,OneByteAddr    ; Test immediate data and direct addr contents
        TIM #123,Index,X        ; Test immediate data and indexed addr contents

        TPA                     ; Acc A = Condition Code Register (CCR), bits 0 - 5

        TRAP                    ; Generates the dreaded TRAP on the Psion!

; ------------------------------------------------------------------------------
; Standard:
        TST A                   ; Test zero or minus, Acc A
        TST B                   ; Test zero or minus, Acc B
; Alternate:
        TSTA                    ; Test zero or minus, Acc A
        TSTB                    ; Test zero or minus, Acc B
; ------------------------------------------------------------------------------

        TST Index,X             ; Test zero or minus, indexed data contents
        TST TwoByteAddr         ; Test zero or minus, extended addr contents

        TSX                     ; Index register = Stack pointer

        TXS                     ; Stack pointer = Index register

        WAI                     ; Wait for interrupt

        XGDX                    ; Exchange Acc D and index registers

END	EQU	$

        DW      END - START     ; Number of bytes used by this "program"

; ******************************************************************************
; ******************************************************************************
;
;            End    of    the    HD6303    Instruction    Set    Examples
;
; ******************************************************************************
; ******************************************************************************
;(end of file)
