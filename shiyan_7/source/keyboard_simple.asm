		ORG 0000H
		LJMP MAIN
		ORG 0100H
MAIN:   LCALL CK
        JB F0 , $-3        ;判断标志位F0(PSW.5)
        MOV A , R1         ;     
        MOV DPTR , #TABLE  ;根据R1参量显示相应的数值
        MOVC A , @A+DPTR   ;
        MOV P1 , A         ;
        LJMP MAIN
CK:     SETB F0            ;设置标志位
        MOV R0 , #0EFH     ;给P3初值，即P3.4=0
        MOV R1 , #00       ;指向TABLE的参量
        MOV R3 , #4        ;扫描行
LOOP1:
        MOV A , R0  
        MOV P3 , A
        NOP
        MOV A  , P3
        SETB C             ;置位C
        MOV R2 , #4        ;判断在哪一列
        LOOP:
        RRC A              ;扫描的数据带C左移一位
        JNC CK1            ;扫描到数据，跳转
        INC R1             ;显示参数加1
        DJNZ R2 , LOOP        
        MOV A , R0         ;
        RL A               ;改变扫描列
        MOV R0 , A         ;
        DJNZ R3 , LOOP1
        LJMP CK             
CK1:    CLR F0             ;复位标志位
        RET        
TABLE:  DB 00,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15   
        END