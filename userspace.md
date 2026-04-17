# Userspace Execution Plan

## Goal

Enable the shell to run in userspace, with ability to execute other userspace binaries on demand. Single userspace process at a time — no scheduling, no multiprocess.

## Current State

- Paging works with identity mapping and demand paging
- Syscalls work via `int 0x80`
- Page fault handler implemented
- Test_syscalls command passes all tests
- Kernel runs at 0x100000 (1MB), identity mapped
- Page directory at 0x104000, page tables at 0x108000

## Requirements

1. **Shell in userspace** — the shell (command interpreter) runs as a user process
2. **User address space** at 0x08000000 (128MB region)
3. **User GDT selectors** with DPL=3
4. **ELF loader** to load shell and other binaries
5. **IRET to user mode** to start shell
6. **Execute other binaries from shell** — load and run, no scheduling

## Implementation Steps

### Step 1: User Address Space Setup

- Add user page tables for 0x08000000 - 0x10000000
- Map user program at 0x08000000, stack at 0x08040000
- Ensure no direct kernel symbol access from user mode

### Step 2: User GDT Entries

- User code selector: base=0x08000000, limit=0xFFFFFFFF, DPL=3, type=0xA
- User data selector: base=0x08000000, limit=0xFFFFFFFF, DPL=3, type=0x2
- TSS for syscall/interrupt stack switching

### Step 3: ELF Loader

- Parse ELF header (magic: 0x7F 'E' 'L' 'F')
- Load PT_LOAD segments to user virtual addresses
- Validate addresses are in user range
- Support loading arbitrary binaries, not just fixed addresses

### Step 4: Shell as Userspace Process

- Move shell execution to user mode at 0x08000000
- Shell receives commands via stdin (read syscall)
- Shell can invoke other binaries by loading ELF and switching to it
- Return to kernel prompt on shell exit (Ctrl-D or exit command)

### Step 5: Execute Binary from Shell

- `execve` syscall: replace current user process with new ELF
- Load new ELF at 0x08000000, set up stack with argc/argv
- IRET to new binary — no return to previous (like Unix exec)

### Step 6: Memory Protection

- User cannot access kernel memory (0x100000+)
- Page faults for user addresses handled appropriately
- Validate all pointer arguments from user space

## Files to Create/Modify

- `src/group_42/src/kernel/user_mode.c` — ELF loader, execve, user entry
- `src/group_42/src/kernel/gdt.c` — Add user GDT selectors
- `src/group_42/include/kernel/user_mode.h` — Header
- `src/group_42/src/shell/shell.c` — May need modifications for userspace
- Build shell as position-independent or link at 0x08000000

## Files to Create for Test

- `user_program.c` — Simple "Hello World" userspace program
- Compile with: `gcc -m32 -nostdlib -static -o user_program user_program.c`

## Testing Approach

1. Build shell as ELF, load at 0x08000000, IRET to user mode
2. Shell prompt appears, accepts commands
3. From shell, run `run /path/to/elf` — loads and executes binary
4. Binary output appears, control returns to shell
5. `exit` from shell returns to kernel prompt

## Out of Scope

- Process scheduling
- Fork system call
- Signal handling
- Multiple simultaneous user processes
- Userspace dynamic allocation (brk) — minimal or none
