ORG 00H   
AJMP MAIN   
ORG 30H 
MAIN: 
MOV SP,#60H
MOV DPTR,#TABLE

KEY: 
ACALL KEY0_1   ;����KEY0_1���ж��Ƿ��м�����
JB F0,$-2   ;�޼����£�תACALL KEY0_1������ɨ��
MOV A,R1   ;R1Ϊȡ��ָ��
MOVC A,@A+DPTR  ;ȡ�룬������ʾ
MOV P1,A
AJMP KEY

KEY0_1:    ;��������ӳ���
SETB F0   ;��F0=1
MOV R3,#0F7H  ;��ɨ��ָ���ֵ��P2.3=0)
MOV R1,#00H   ;ȡ��ָ���ֵ
L2: 
MOV A,R3   ;����ɨ��ָ��
MOV P2,A   ;�����P2����ʼɨ��Ϊ0��һ��
NOP
MOV A,P2   ;����P2
SETB C   
MOV R5,#4   ;���P2.7~P2.4,��4 ��
L3:    ;���4��
RLC A   ;����һλ(P2.7~P2.4)
JNC KEY1   ;��⵽C=0����ʾ������
INC R1   ;�޼�������ȡ��ָ���1
DJNZ R5,L3   ;4�м����ϣ�
MOV A,R3   ;����ɨ��ָ��
SETB C
RRC A       ;ɨ��Ϊ0����һ��,
MOV R3,A   ;���R3ɨ��ָ��Ĵ���
JC L2   ;C=0����ɨ�����
RET

KEY1: 
CLR F0   ;F0��0 ����ʾ��������
RET   

TABLE:  
DB      0C0H;0
DB      0F9H;1
DB      0A4H;2
DB      0B0H;3
DB      099H;4
DB      092H;5
DB      082H;6
DB      0F8H;7
DB      080H;8
DB      090H;9
DB      088H;A
DB      083H;b
DB      0C6H;C
DB      0A1H;d
DB      086H;E
DB      08EH;F

END    