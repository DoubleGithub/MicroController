
		;ʵʱʱ��ʵ�� 
        PORT EQU 0CFA0H  
             
        BUF EQU 23H ;��ż���ֵ 
        SBF EQU 22H ;�����ֵ 
        MBF EQU 21H ;��ŷ�ֵ 
		HBF EQU 20H;���Сʱ 
        ORG 0000H 
        LJMP START 
        ORG 401BH 
        LJMP CLOCK 
        ORG 4100H 
START: 	MOV R0,#40H ;40H-45H����ʾ�����������δ�����λ�����λ
					;�Լ��ָ�λ���ֵ�λ�Լ�Сʱ��λ��Сʱ��λ 
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

        MOV TMOD,#10H ;��ʱ��1��ʼ��Ϊ��ʽ1 
        MOV TH1,#38H ;��ʱ�䳣������ʱ0.1�� 
        MOV TL1,#00H 

        MOV BUF,#00H ;��0 
        MOV SBF,#00H 
        MOV MBF,#00H 
		MOV HBF,#00H 

        SETB ET1 
        SETB EA 
        SETB TR1 

   DS1: MOV R0,#40H ;����ʾ��������ַ 
        MOV R2,#20H ;��ɨ���ֵ,��������ߵ�LED6 
   DS2: MOV DPTR,#PORT  
        MOV A,@R0 ;�õ��Ķ���������������ݿ� 
        ACALL TABLE 
        MOVX @DPTR,A  

        MOV A,R2 ;��λ���ݿ�P1���λ���� 
        CPL A 
        MOV P1,A  

        MOV R3,#0FFH ;��ʱһС��ʱ�� 
   DEL: NOP 
        DJNZ R3,DEL
        
        MOV P1 , 0FFH   ;������Ӱ���� 

        INC R0 ;��ʾ�����ֽڼ�һ 
        CLR C 
        MOV A,R2 
        RRC A ;��������һλ 
        MOV R2,A ;��ĩһλ�Ƿ���ʾ���?,������ 
        JNZ DS2 ;����������ʾ 

        MOV R0,#45H 
        MOV A,SBF ;����ֵ�ֱ����44H,45H�� 
        ACALL GET 

        MOV A,MBF ;�ѷ�ֵ�ֱ����42H,43H�� 
        ACALL GET 

		MOV A, HBF;��Сʱ�ֱ����40H,41H�� 
		ACALL  GET  
        SJMP DS1 ;תDS1��ͷ��ʾ�� 
 TABLE: INC A ;ȡ�����ֶ�Ӧ�Ķ��� 
        MOVC A,@A+PC 
        RET 
        DB 3FH,06H,5BH,4FH,66H,6DH,7DH, 07H, 7FH,6FH, 40H 

   GET: MOV R1,A ;�Ѵ�ʱ���ֻ����ֽ���ȡ����ֵ�ĸ� 
        ANL A,#0FH ;λ���ε�,�����뻺���� 
        MOV @R0,A 

        DEC R0 
        MOV A,R1 ;�Ѵ�ʱ���ֻ����ֽ���ȡ����ֵ�ĵ� 
        SWAP A ;λ���ε�,�����뻺���� 
        ANL A,#0FH 
        MOV @R0,A 
        DEC R0 ;R0ָ������һλ 
        RET 
 CLOCK: MOV TL1,#038H ;��ʱ�䳣�� 
        MOV TH1,#00H 
        PUSH PSW  
             
        PUSH ACC 
        INC BUF ;������һ 
        MOV A,BUF ;�Ƶ�10��?û����ת��QUIT�˳��ж� 
        CJNE A,#0AH,QUIT 
        MOV BUF,#00H ;�ó�ֵ 
        MOV A,SBF 
        MOV A , #1 ;��ֵ��һ,��ʮ���Ƶ�������� 
        DA A ;���ֽ� 
        MOV SBF,A 
        CJNE A,#60H,QUIT ;�Ƶ�60��?û����ת��QUIT�˳��ж� 
        MOV SBF,#00H ;��,���ֽ����� 
        MOV A,MBF 
        MOV A , #1 ;��ֵ��һ,��ʮ���Ƶ�������� 
        DA A ;���ֽ� 
        MOV MBF,A 
        CJNE A,#60H,QUIT ;��ֵΪ60��?�������˳��ж� 
        MOV MBF,#00H ;��,���� 
		MOV  A,HBF 
		MOV A , #1; 
		DA  A 
		MOV  HBF,A 
		CJNE A,#24H,QUIT   ;ʱֵΪ24��,�������˳��ж� 
QUIT: 	POP ACC 
		POP PSW 
		RETI ;�жϷ��� 
        END