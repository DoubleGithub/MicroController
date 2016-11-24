ORG 00H   
AJMP MAIN   
ORG 30H 
MAIN: 
MOV SP,#60H
MOV DPTR,#TABLE

KEY: 
ACALL KEY0_1   ;调用KEY0_1，判断是否有键按下
JB F0,$-2   ;无键按下，转ACALL KEY0_1，继续扫描
MOV A,R1   ;R1为取码指针
MOVC A,@A+DPTR  ;取码，关送显示
MOV P1,A
AJMP KEY

KEY0_1:    ;按键检测子程序
SETB F0   ;设F0=1
MOV R3,#0F7H  ;行扫描指针初值（P2.3=0)
MOV R1,#00H   ;取码指针初值
L2: 
MOV A,R3   ;载入扫描指针
MOV P2,A   ;输出至P2，开始扫描为0的一行
NOP
MOV A,P2   ;读入P2
SETB C   
MOV R5,#4   ;检测P2.7~P2.4,共4 列
L3:    ;检测4列
RLC A   ;左移一位(P2.7~P2.4)
JNC KEY1   ;检测到C=0，表示被按下
INC R1   ;无键按下则取码指针加1
DJNZ R5,L3   ;4列检测完毕？
MOV A,R3   ;载入扫描指针
SETB C
RRC A       ;扫描为0的下一行,
MOV R3,A   ;存回R3扫描指针寄存器
JC L2   ;C=0，行扫描完毕
RET

KEY1: 
CLR F0   ;F0清0 ，表示按键按下
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