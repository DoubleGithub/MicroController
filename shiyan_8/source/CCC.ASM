org 0000h
ajmp main
org 0040h
main:
mov 7fh,#99h
acall display
loop:
acall display
acall delay_1s
mov a,7fh
jz out
add a,#99h
da a
mov 7fh,a
ajmp loop
out:sjmp $
display:
mov a,7fh
anl a,0f0h
swap a
mov dptr,#table
movc a,@a+dptr
mov p0,a
mov a,7fh
anl a,0fh
movc a,@a+dptr
mov p2,a
ret
table: db 3fh,06h,5bh,4fh,66h,6dh,7dh,07h,7fh,6fh
delay_1s:
mov r7,#100
del:mov r6,#20
de:mov r5,#248
djnz r5,$
djnz r6,de
djnz r7,del
ret
end