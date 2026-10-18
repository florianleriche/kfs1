BITS 32

; Header Multiboot 1

section .multiboot
align 4

MB_MAGIC    equ 0x1BADB002   ; valeur que GRUB cherche pour reconnaître un kernel multiboot
MB_FLAGS    equ 0            ; aucune option demandée à GRUB
MB_CHECKSUM equ -(MB_MAGIC + MB_FLAGS) ; magic + flags + checksum doit faire 0

    dd MB_MAGIC
    dd MB_FLAGS
    dd MB_CHECKSUM


; Pile du kernel

section .bss
align 16

stack_bottom:
    resb 16384
stack_top:


; Point d'entrée du kernel

section .text
global _start
extern kmain

_start:
    cli                  ; coupe les interruptions, rien n'est encore prêt pour les gérer
    mov esp, stack_top   ; la pile du host n'est pas garantie valide, on pose la nôtre
    call kmain            ; saute dans le code C

.hang:
    hlt                   ; si kmain revient, on arrête le CPU proprement
    jmp .hang

section .note.GNU-stack noalloc noexec nowrite progbits
