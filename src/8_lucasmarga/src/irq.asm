BITS 32

EXTERN irq_handler

GLOBAL irq0
GLOBAL irq1
GLOBAL irq2
GLOBAL irq3
GLOBAL irq4
GLOBAL irq5
GLOBAL irq6
GLOBAL irq7
GLOBAL irq8
GLOBAL irq9
GLOBAL irq10
GLOBAL irq11
GLOBAL irq12
GLOBAL irq13
GLOBAL irq14
GLOBAL irq15

irq0:
    push dword 0
    call irq_handler
    add esp, 4
    iret

irq1:
    push dword 1
    call irq_handler
    add esp, 4
    iret

irq2:
    push dword 2
    call irq_handler
    add esp, 4
    iret

irq3:
    push dword 3
    call irq_handler
    add esp, 4
    iret

irq4:
    push dword 4
    call irq_handler
    add esp, 4
    iret

irq5:
    push dword 5
    call irq_handler
    add esp, 4
    iret

irq6:
    push dword 6
    call irq_handler
    add esp, 4
    iret

irq7:
    push dword 7
    call irq_handler
    add esp, 4
    iret

irq8:
    push dword 8
    call irq_handler
    add esp, 4
    iret

irq9:
    push dword 9
    call irq_handler
    add esp, 4
    iret

irq10:
    push dword 10
    call irq_handler
    add esp, 4
    iret

irq11:
    push dword 11
    call irq_handler
    add esp, 4
    iret

irq12:
    push dword 12
    call irq_handler
    add esp, 4
    iret

irq13:
    push dword 13
    call irq_handler
    add esp, 4
    iret

irq14:
    push dword 14
    call irq_handler
    add esp, 4
    iret

irq15:
    push dword 15
    call irq_handler
    add esp, 4
    iret