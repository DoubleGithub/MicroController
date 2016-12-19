		ORG 0000H
		LJMP MAIN
		ORG 0100H
MAIN:	MOV R2 , #0F1H
		MOV R3 , #23H
		MOV R6 , #0F4H
		MOV R7 , #56H
		LCALL ADDER
		SJMP $
ADDER:	MOV B  , R7
		MOV A  , R3 
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
		MOV A , R3
		ADDC A , B
		MOV R3, A
		CLR A
		RLC A
		MOV R0, A
		
		MOV A , R2
		MOV B , R6
		MUL AB
		ADD A , R3
		MOV R3, A
		MOV A , B
		ADDC A , R0
		MOV R2,A
		SJMP $		
		END