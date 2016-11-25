		;60秒倒计时
		;按键连接在P3.2
;---------------------------------------
        ORG  0000H
        LJMP MAIN
        ORG  0003H
        LJMP X0_INT
        ORG  000BH
        LJMP T0_INT
;---------------------------------------
        ORG  0100H
MAIN:                            ;初始化
        MOV  TMOD, #01H
        MOV  TL0, #(65536 - 50000) MOD 256
        MOV  TH0, #(55536 - 50000) / 256
        SETB TR0
        SETB ET0
        SETB EX0
        SETB EA
        MOV  DPTR, #TAB          ;表首址送DPTR
        MOV  R1, #0
        MOV  R2, #0
        MOV  R3, #20
;---------------------------------------
M_LOOP:
        CALL DIS
        SJMP M_LOOP
;---------------------------------------
T0_INT:
        MOV  TL0, #(65536 - 50000) MOD 256
        MOV  TH0, #(55536 - 50000) / 256
        DJNZ R3, T0_E
        MOV  R3, #20
        DEC  R1
        CJNE R1, #255, T0_E
        MOV  R1, #9
        DEC  R2
        CJNE R2, #255, T0_E
        MOV  R2, #5
T0_E:
        RETI
;---------------------------------------
X0_INT:
        CALL DIS
        JNB  P3.2, X0_INT
        CPL  TR0
        RETI
;---------------------------------------
DIS:                             ;显示程序.
        MOV  A,  R2              ;显示十位.
        MOVC A,  @A+DPTR
		
		MOV  P0, A
        ;CLR  P2.1
        ;MOV  P1, A
        ;SETB P2.0                ;十位的位选.
        ;CALL DLY
        MOV  A,  R1              ;个位显示.
        MOVC A,  @A+DPTR
		
		MOV  P2, A
        ;CLR  P2.0                ;关闭十位的显示.
        ;MOV  P1, A
        ;SETB P2.1                ;个位的位选.
        ;CALL DLY
        RET
;------------------------------
TAB:  DB 3FH,06H,5BH,4FH,66H,6DH,7DH,07H
        DB 7FH,6FH,77H,7CH,39H,5EH,79H,71H
;TAB:    DB  0xc0,0xf9,0xa4,0xb0,0x99  ;共阳极段码 0~4
       ; DB  0x92,0x82,0xf8,0x80,0x90  ;共阳极段码 5~9
;---------------------------------------
DLY:    MOV  R6, #20            ;10ms延时.
D1:     MOV  R7, #248
        DJNZ R7, $
        DJNZ R6, D1
        RET
;---------------------------------------
end