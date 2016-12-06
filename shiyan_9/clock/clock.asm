
		;实时时钟实验 
        PORT EQU 0CFA0H  
             
        BUF EQU 23H ;存放计数值 
        SBF EQU 22H ;存放秒值 
        MBF EQU 21H ;存放分值 
		HBF EQU 20H;存放小时 
        ORG 0000H 
        LJMP START 
        ORG 401BH 
        LJMP CLOCK 
        ORG 4100H 
START: 	MOV R0,#40H ;40H-45H是显示缓冲区，依次存放秒高位、秒低位
					;以及分高位、分底位以及小时高位、小时低位 
        MOV A,#00H ;  
        MOV @R0,A ;  
        INC R0 
        MOV @R0,A 
        INC R0 

        MOV @R0,A 
        INC R0 
        MOV @R0,A 
        INC R0 
        MOV @R0,A 
        INC R0 
        MOV @R0,A 

        MOV TMOD,#10H ;定时器1初始化为方式1 
        MOV TH1,#38H ;置时间常数延时0.1秒 
        MOV TL1,#00H 

        MOV BUF,#00H ;置0 
        MOV SBF,#00H 
        MOV MBF,#00H 
		MOV HBF,#00H 

        SETB ET1 
        SETB EA 
        SETB TR1 

   DS1: MOV R0,#40H ;置显示缓冲区首址 
        MOV R2,#20H ;置扫描初值,点亮最左边的LED6 
   DS2: MOV DPTR,#PORT  
        MOV A,@R0 ;得到的段显码输出到段数据口 
        ACALL TABLE 
        MOVX @DPTR,A  

        MOV A,R2 ;向位数据口P1输出位显码 
        CPL A 
        MOV P1,A  

        MOV R3,#0FFH ;延时一小段时间 
   DEL: NOP 
        DJNZ R3,DEL
        
        MOV P1 , 0FFH   ;消除拖影现象 

        INC R0 ;显示缓冲字节加一 
        CLR C 
        MOV A,R2 
        RRC A ;显码右移一位 
        MOV R2,A ;最末一位是否显示完毕?,如无则 
        JNZ DS2 ;继续往下显示 

        MOV R0,#45H 
        MOV A,SBF ;把秒值分别放于44H,45H中 
        ACALL GET 

        MOV A,MBF ;把分值分别放入42H,43H中 
        ACALL GET 

		MOV A, HBF;把小时分别放入40H,41H中 
		ACALL  GET  
        SJMP DS1 ;转DS1从头显示起 
 TABLE: INC A ;取与数字对应的段码 
        MOVC A,@A+PC 
        RET 
        DB 3FH,06H,5BH,4FH,66H,6DH,7DH, 07H, 7FH,6FH, 40H 

   GET: MOV R1,A ;把从时、分或秒字节中取来的值的高 
        ANL A,#0FH ;位屏蔽掉,并送入缓冲区 
        MOV @R0,A 

        DEC R0 
        MOV A,R1 ;把从时、分或秒字节中取来的值的低 
        SWAP A ;位屏蔽掉,并送入缓冲区 
        ANL A,#0FH 
        MOV @R0,A 
        DEC R0 ;R0指针下移一位 
        RET 
 CLOCK: MOV TL1,#038H ;置时间常数 
        MOV TH1,#00H 
        PUSH PSW  
             
        PUSH ACC 
        INC BUF ;计数加一 
        MOV A,BUF ;计到10否?没有则转到QUIT退出中断 
        CJNE A,#0AH,QUIT 
        MOV BUF,#00H ;置初值 
        MOV A,SBF 
        MOV A , #1 ;秒值加一,经十进制调整后放入 
        DA A ;秒字节 
        MOV SBF,A 
        CJNE A,#60H,QUIT ;计到60否?没有则转到QUIT退出中断 
        MOV SBF,#00H ;是,秒字节清零 
        MOV A,MBF 
        MOV A , #1 ;分值加一,经十进制调整后放入 
        DA A ;分字节 
        MOV MBF,A 
        CJNE A,#60H,QUIT ;分值为60否?不是则退出中断 
        MOV MBF,#00H ;是,清零 
		MOV  A,HBF 
		MOV A , #1; 
		DA  A 
		MOV  HBF,A 
		CJNE A,#24H,QUIT   ;时值为24否,不是则退出中断 
QUIT: 	POP ACC 
		POP PSW 
		RETI ;中断返回 
        END