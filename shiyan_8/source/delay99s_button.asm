		ORG 0000H
		LJMP MAIN
		ORG 0100H
MAIN:	MOV R2 , #00			  ;按键次数
MAIN1:	MOV R0 , #99              ;倒计时秒数
		MOV R1 , #100             ;循环次数		
		MOV DPTR , #TABLE
		MOV P0 , #6FH
		MOV p2 , #6FH

KEYCHECK:
		JNB P3.5 , TMP
		MOV A , R2
		JZ KEYCHECK
		LJMP S1
		TMP:	
		LCALL DELAY10MS	
		JNB P3.5 , KEYCHECK
		INC R2				
		S1:MOV A ,R2
		MOV B ,#3
		DIV AB
		MOV A , B
		X1:CJNE A , #1 , X2
		LJMP NEXT
		X2:CJNE A , #2 , X3
		LJMP KEYCHECK
		X3:LJMP MAIN1
NEXT:	
		MOV A , R0
		MOV B , #10
		DIV AB
		MOVC A,@A+DPTR
		MOV P0 , A
		MOV A , B
		MOVC A,@A+DPTR
		MOV P2 , A
		LCALL DELAY1S
		MOV A , R2
		DEC R0
		;DJNZ R1 , NEXT
		DJNZ R1 , KEYCHECK
		SJMP $
		
DELAY1S:   ;误差 0us
		;MOV R7,#167
		MOV R7,#80
		DL0:
		MOV R6,#171
		DL1:
		MOV R5,#16
		;DJNZ R5,$
		DL2:
		Z1:JNB P3.5 , RETURN
		DJNZ R5,DL2
		DJNZ R6,DL1
		DJNZ R7,DL0
		NOP
		RET
		RETURN:
		LCALL DELAY10MS
		JNB P3.5 , Z1
		INC R2
		RET
	
DELAY10MS:   ;误差 0us
    MOV R7,#01H
	DL3:
    MOV R6,#26H
	DL4:
    MOV R5,#82H
    DJNZ R5,$
    DJNZ R6,DL4
    DJNZ R7,DL3
    RET
TABLE:  DB 3FH,06H,5BH,4FH,66H,6DH,7DH,07H
        DB 7FH,6FH,77H,7CH,39H,5EH,79H,71H
		END