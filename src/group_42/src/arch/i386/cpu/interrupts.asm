[extern isr_handler]
[extern irq_handler]
[extern syscall_handler]

isr_common_stub:
	pusha			; Pushes edi, esi, ebp, esp, ebx, edx, edx, eax

	mov ax, ds		; Lower 16-bits of eax = ds.
	push eax		; save the data segment descriptor

	mov ax, 0x10	; load the kernel data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

    push esp
    cld
	call isr_handler
	add esp, 4

	pop eax			; reload the original data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	popa			; Pops edi, esi, ebp...
	add esp, 8		; Cleans up the pushed error code and pushed ISR number
	sti
	iret			; pops 5 things at once, CS, EIP, EFLAGS, SS, and ESP


irq_common_stub:
	pusha			; Pushes edi, esi, ebp, esp, ebx, edx, edx, eax

	mov ax, ds		; Lower 16-bits of eax = ds.
	push eax		; save the data segment descriptor

	mov ax, 0x10	; load the kernel data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

    push esp
    cld
	call irq_handler
	add esp, 4

	pop eax			; reload the original data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	popa			; Pops edi, esi, ebp...
	add esp, 8		; Cleans up the pushed error code and pushed ISR number
	sti
	iret			; pops 5 things at once, CS, EIP, EFLAGS, SS, and ESP


; defines all CPU defined ISRs
%macro isr_err_stub 1
isr_stub_%+%1:
    CLI
    PUSH BYTE %1
    JMP isr_common_stub
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    CLI
    PUSH BYTE 0
    PUSH BYTE %1
    JMP isr_common_stub
%endmacro

%macro irq 2
irq_stub_%+%1:
    CLI
    PUSH BYTE 0
    PUSH BYTE %2
    JMP irq_common_stub
%endmacro

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31
isr0: jmp isr_stub_0
isr1: jmp isr_stub_1
isr2: jmp isr_stub_2
isr3: jmp isr_stub_3
isr4: jmp isr_stub_4
isr5: jmp isr_stub_5
isr6: jmp isr_stub_6
isr7: jmp isr_stub_7
isr8: jmp isr_stub_8
isr9: jmp isr_stub_9
isr10: jmp isr_stub_10
isr11: jmp isr_stub_11
isr12: jmp isr_stub_12
isr13: jmp isr_stub_13
isr14: jmp isr_stub_14
isr15: jmp isr_stub_15
isr16: jmp isr_stub_16
isr17: jmp isr_stub_17
isr18: jmp isr_stub_18
isr19: jmp isr_stub_19
isr20: jmp isr_stub_20
isr21: jmp isr_stub_21
isr22: jmp isr_stub_22
isr23: jmp isr_stub_23
isr24: jmp isr_stub_24
isr25: jmp isr_stub_25
isr26: jmp isr_stub_26
isr27: jmp isr_stub_27
isr28: jmp isr_stub_28
isr29: jmp isr_stub_29
isr30: jmp isr_stub_30
isr31: jmp isr_stub_31

irq  0,  32
irq  1,  33
irq  2,  34
irq  3,  35
irq  4,  36
irq  5,  37
irq  6,  38
irq  7,  39
irq  8,  40
irq  9,  41
irq 10,  42
irq 11,  43
irq 12,  44
irq 13,  45
irq 14,  46
irq 15,  47

global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15
irq0: jmp irq_stub_0
irq1: jmp irq_stub_1
irq2: jmp irq_stub_2
irq3: jmp irq_stub_3
irq4: jmp irq_stub_4
irq5: jmp irq_stub_5
irq6: jmp irq_stub_6
irq7: jmp irq_stub_7
irq8: jmp irq_stub_8
irq9: jmp irq_stub_9
irq10: jmp irq_stub_10
irq11: jmp irq_stub_11
irq12: jmp irq_stub_12
irq13: jmp irq_stub_13
irq14: jmp irq_stub_14
irq15: jmp irq_stub_15

global isr_stub_table
isr_stub_table:
    %assign i 0
    %rep    32
        DD isr_stub_%+i
    %assign i i+1
    %endrep

global irq_stub_table
irq_stub_table:
    %assign i 0
    %rep    16
        DD irq_stub_%+i
    %assign i i+1
    %endrep

global syscall_stub
syscall_stub:
    CLI
    PUSH 0
    PUSH 0x80
    PUSHA

    mov ebx, esp
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push ebx
    cld
    call syscall_handler
    add esp, 4

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    POPA
    add esp, 8
    iret
