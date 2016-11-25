	ORG 0000H
MAIN:	MOV R2 , #0F1H
		MOV R3 , #23H
		MOV R6 , #0F4H
		MOV R7 , #56H
		MOV A  , R3 
		MOV B  , R7
		MUL AB
		MOV R4 , B
		MOV R5 , A
		
		MOV A  , R3
		MOV B  , R6
		MUL AB
		ADD A  , R4
		MOV R4 , A
		CLR A
		ADDC A , B
		MOV R3 , A
		
		MOV A , R2 
		MOV B , R7
		MUL AB 
		ADD A , R4
		MOV R4,A
		MOV 200H , B		
		MOV A , #00H
		MOV B , #00H
		ADDC A, B 
		MOV 201H , A			
		MOV A , 200H
		MOV B , 201H
		ADD A , B
		ADD A , R3
		MOV R3, A
		MOV R0 , 201H

		MOV A , R2
		MOV B , R6
		MUL AB
		ADD A , R3
		MOV R3, A
		MOV A , B
		ADD A , R0
		MOV R2,A
		
		
		SJMP $
			END