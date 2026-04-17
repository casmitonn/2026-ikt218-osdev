global load_cr3
load_cr3:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

global enable_paging
enable_paging:
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax
    ret

global invalidate_page
invalidate_page:
    mov eax, [esp + 4]
    invlpg [eax]
    ret

global tss_flush
tss_flush:
    mov ax, 0x28
    ltr ax
    ret
