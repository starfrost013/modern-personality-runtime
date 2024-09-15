; ****** Dev16 ******
; Copyright © 2022-2024 starfrost. See licensing information in the licensing file
;
; mov.asm                          15 September 2024
; Test suite for the Dev16 runtime [MOV] instruction
test_mov:
    mov ax, 9810h
    mov es, ax
    mov ax, es
    mov bx, ax

    ; SUCCESS BX=AX=ES=9810h
    ; FAILURE...ANYTHING ELSE
