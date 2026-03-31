# Assignment 4: Memory and PIT

## 1. Introduction

This assignment focused on extending the `3_jakub` operating system with two core kernel features: basic memory management and a programmable interval timer (PIT) driver. The goal was to make dynamic allocation possible inside the kernel, enable paging, and add timer-based sleep functions that could be used both as busy waiting and as interrupt-driven waits.

The implementation was carried out using a structured workflow. The provided assignment templates in `include/assignment_files` were reviewed first, then adapted to fit the existing codebase instead of being copied directly. This was important because the templates assumed headers and support code that did not exist in the current kernel project.

## 2. Starting Point

At the start of the task, the live kernel already supported:

- GDT setup
- IDT setup
- ISR and IRQ registration
- keyboard interrupt handling
- a minimal C and C++ kernel entry flow

However, Assignment 4 functionality was not yet integrated into the active build. The relevant template files existed only under `include/assignment_files`, and none of them were compiled by the active kernel target in `CMakeLists.txt`.

The starting codebase also had a very small local libc implementation. Because of that, the assignment templates needed adjustment in several places:

- template includes such as `<libc/system.h>` did not exist in this project
- the current `printf` implementation was minimal
- the live build only included the current `src/3_jakub/src` tree, not the template files

The most important attached files that were used as reference material were:

- `include/assignment_files/kernel/include/kernel/memory.h`
- `include/assignment_files/kernel/include/kernel/pit.h`
- `include/assignment_files/kernel/src/memory/malloc.c`
- `include/assignment_files/kernel/src/memory/memory.c`
- `include/assignment_files/kernel/src/memory/memutils.c`
- `include/assignment_files/apps/song/src/song.cpp`

These files were treated as specification hints and starter code, not as drop-in code. That distinction mattered throughout the task, because several of the provided files assumed a surrounding kernel structure that `3_jakub` did not already have.

## 3. Planning and Design

The design goal was to keep the solution assignment-sized and stable, while still making a few careful improvements where they added value without much complexity.

The implementation followed these design choices:

- keep the public API in `include/kernel/` to match the assignment structure
- group memory-related logic under `src/memory/`
- implement PIT as a small IRQ0-driven module in `src/pit.c`
- preserve the existing interrupt framework instead of replacing it
- use a simple first-fit heap allocator
- keep paging deliberately simple with identity mapping of the first 8 MB

This kept the solution easy to reason about while still satisfying the assignment requirements.

In a later polish pass, the terminal and keyboard behavior were also improved so the boot output was cleaner and the heap could be demonstrated interactively.

The mapping between the attached files and the final implementation was:

- `include/assignment_files/kernel/include/kernel/memory.h` -> adapted into `include/kernel/memory.h`
- `include/assignment_files/kernel/include/kernel/pit.h` -> adapted into `include/kernel/pit.h`
- `include/assignment_files/kernel/src/memory/malloc.c` -> adapted into `src/memory/malloc.c`
- `include/assignment_files/kernel/src/memory/memory.c` -> adapted into `src/memory/memory.c`
- `include/assignment_files/kernel/src/memory/memutils.c` -> adapted into `src/memory/memutils.c`

The optional song example in `include/assignment_files/apps/song/src/song.cpp` was not integrated directly, but it was still useful as evidence of the intended role of the PIT API and the `sleep_interrupt()` function.

## 4. Implemented Files

The following new files were added:

- `include/kernel/memory.h`
- `include/kernel/pit.h`
- `src/memory/memutils.c`
- `src/memory/malloc.c`
- `src/memory/memory.c`
- `src/pit.c`

The following existing files were updated:

- `src/kernel.c`
- `src/kernel.cpp`
- `CMakeLists.txt`
- `scripts/start_qemu.sh`
- `include/libc/stdio.h`
- `include/libc/string.h`
- `include/interrupts/keyboard.h`
- `src/libc/stdio.c`
- `src/interrupts/keyboard.c`
- `src/multiboot2.asm`

## 5. Memory Management Implementation

### 5.1 Kernel Memory Initialization

The linker already exposed the symbol `end` in `src/arch/i386/linker.ld`. This symbol marks the end of the kernel image in memory, so it was used as the anchor for the heap start.

In `kernel.c`, the required line

```c
extern uint32_t end;
```

was added below the includes. During boot, the kernel now calls:

```c
init_kernel_memory(&end);
init_paging();
print_memory_layout();
```

This matches the assignment requirement and ensures that the allocator and paging are initialized before the timer is used.

The overall structure came from the provided `include/assignment_files/kernel/include/kernel/memory.h`, but the include paths and declarations were adapted to the local kernel and local libc layout.

### 5.2 Heap Allocator

The allocator in `src/memory/malloc.c` is a simple first-fit allocator using a small allocation header:

- `status` marks whether a block is in use
- `size` stores the payload size

The allocator supports:

- `malloc(size_t size)`
- `free(void *mem)`
- reuse of freed blocks
- splitting of larger free blocks
- zero-initialization of allocated memory

Some quality improvements were added compared to the raw template:

- the template pointer arithmetic bug on `kernel_end + 0x1000` was corrected so the heap start is computed in bytes, not in `uint32_t` units
- `free()` now safely ignores `NULL`
- adjacent free blocks are merged to reduce fragmentation

These changes were based directly on issues visible in `include/assignment_files/kernel/src/memory/malloc.c`, especially the layout assumptions around `kernel_end`, the lack of safer edge handling in `free()`, and the very simple allocation reuse logic in the starter version.

### 5.3 Page-Aligned Allocation

The page-aligned allocator is implemented through:

- `pmalloc(size_t size)`
- `pfree(void *mem)`

This allocator uses a small descriptor array for fixed page-sized allocations near the 4 MB boundary. Unlike the template, the implementation supports multi-page requests by searching for a contiguous free run of pages.

This was a deliberate improvement over the attached `include/assignment_files/kernel/src/memory/malloc.c`, where the baseline `pmalloc()` logic effectively treated all page allocations as single-page reservations.

### 5.4 Memory Utility Functions

The file `src/memory/memutils.c` provides:

- `memcpy`
- `memset`
- `memset16`

These functions are used by the allocator and paging setup and keep the memory module self-contained.

The implementation here stayed close to `include/assignment_files/kernel/src/memory/memutils.c`, because that attached file already matched the level of simplicity needed for the assignment.

## 6. Paging Implementation

Paging is implemented in `src/memory/memory.c`.

The implementation was guided by `include/assignment_files/kernel/src/memory/memory.c`, which already suggested a fixed page directory region and a simple identity-mapping strategy.

The design is intentionally simple:

- the page directory is placed at `0x400000`
- page tables begin at `0x401000`
- the first 8 MB are identity-mapped

This means:

- virtual `0x00000000` to `0x003FFFFF` maps to physical `0x00000000` to `0x003FFFFF`
- virtual `0x00400000` to `0x007FFFFF` maps to physical `0x00400000` to `0x007FFFFF`

This layout is enough for the assignment because:

- the kernel is loaded at 1 MB
- the general heap lives below 4 MB
- paging structures live at and above 4 MB

The function `paging_enable()` loads the page directory into `cr3` and sets the paging bit in `cr0`.

The final version kept the template strategy but adapted it to the current kernel’s initialization flow and local headers.

## 7. PIT Implementation

The PIT driver is implemented in `src/pit.c`.

The public API and constants were adapted from `include/assignment_files/kernel/include/kernel/pit.h`, while the intended runtime use was cross-checked against `include/assignment_files/apps/song/src/song.cpp`.

The main features are:

- programming PIT channel 0 using the control port `0x43`
- registering an IRQ0 callback through the existing interrupt handler system
- maintaining a global `volatile uint32_t pit_ticks`
- exposing `pit_get_ticks()`
- implementing `sleep_busy(uint32_t milliseconds)`
- implementing `sleep_interrupt(uint32_t milliseconds)`

The timer is configured to `1000 Hz`, which makes one tick correspond to roughly one millisecond. This made the sleep functions straightforward:

- `sleep_busy()` spins in a loop until enough ticks have passed
- `sleep_interrupt()` waits using `sti` and `hlt`, allowing the CPU to sleep until the next interrupt

This difference is important. `sleep_busy()` consumes CPU time while waiting, but `sleep_interrupt()` explicitly yields until the next interrupt. In the final code, this behavior is implemented in `src/pit.c` with a loop that repeatedly executes `sti` followed by `hlt`, which matches the assignment’s requirement for interrupt-driven sleeping.

## 8. Kernel Integration

The kernel startup sequence in `src/kernel.c` was updated so the new subsystems are initialized in a safe order:

1. validate Multiboot2 magic
2. initialize GDT
3. initialize IDT
4. initialize ISRs
5. initialize IRQs
6. initialize kernel memory
7. initialize paging
8. print memory layout
9. register keyboard handler
10. initialize PIT
11. enable interrupts with `sti`

After initialization, the kernel now performs visible Assignment 4 checks:

- three `malloc()` allocations
- one `free()` followed by another `malloc()`
- a busy-wait sleep test
- an interrupt-driven sleep test

The C++ side in `src/kernel.cpp` now provides:

- `operator new`
- `operator new[]`
- `operator delete`
- `operator delete[]`
- sized delete overloads

This satisfies the assignment requirement to allocate memory with the overloaded `new` operator. A small C++ object allocation test was added and prints a computed value to confirm that the object was allocated, written to, and read back correctly.

## 9. Debugging and Stability Fixes

The final solution did not appear fully formed in one step. Several bugs appeared during integration, and they had to be traced back to specific code paths.

### 9.1 C and C++ Linkage Fixes

One of the early integration issues was that C++ code in `src/kernel.cpp` used `printf()`, but the local libc headers had no C linkage guards. That caused the linker to look for mangled C++ versions of `printf()` and related libc symbols. The fix was to wrap the declarations in `include/libc/stdio.h` and `include/libc/string.h` in `extern "C"` when compiled as C++.

This issue did not come from the assignment templates directly, but it became relevant as soon as the assignment requirement to support allocation through overloaded `new` was implemented in `src/kernel.cpp`.

### 9.2 VGA Display Corruption and Clear-Screen Fix

After the first memory/PIT integration, the boot screen sometimes showed red and blue blocks instead of clean text. The root cause was that the kernel wrote text into VGA memory without first initializing the terminal state and clearing the visible screen.

The fix was implemented in `src/libc/stdio.c`:

- `terminal_initialize()` resets cursor state
- the terminal buffer is filled with blank text cells
- the visible 80x25 region is rendered explicitly to VGA memory

This behavior is now visible both during boot and after running the `clear` command in the CLI.

### 9.3 Page Fault Investigation and Fix

At one stage the system booted into repeated page faults (`interrupt 14`). The key debugging observation was that `_start` in `src/multiboot2.asm` called `main`, but there was no safe code path after `main()` returned. That meant the CPU fell through into unintended instructions after the kernel had finished its main path, which is exactly the kind of control-flow bug that can quickly produce page faults.

The fix was to add an explicit hang loop after `call main` in `src/multiboot2.asm`.

That fix matters for the report because it explains why the system could still fault even though the page directory setup in `src/memory/memory.c` and the heap setup in `src/memory/malloc.c` were otherwise valid.

### 9.4 Interrupt-Friendly Idle Loop

After the new hang loop was added, text input stopped working. The reason was that the first version of the hang loop used `cli`, which disabled interrupts before halting. That prevented keyboard IRQ1 from firing while the kernel was idle.

The final idle loop in `src/multiboot2.asm` now uses:

- `sti`
- `hlt`
- `jmp` back to the loop

This small change made the system behave much more like a real interrupt-driven kernel: it stays idle, but still wakes correctly on keyboard and timer interrupts.

## 10. User-Visible Proof of Memory Use and CLI

To make heap activity visible in a practical way, the keyboard input path was extended with a small heap-backed history feature.

The behavior is:

- type text and press Enter to save the line on the heap
- type `history` to print the saved entries
- type `clearhistory` to free all stored entries

Each stored line is copied into dynamically allocated memory with `malloc()`, and the history system frees those allocations again when old entries are rotated out or when `clearhistory` is used. This gives a direct and interactive proof that memory allocation and freeing are both working inside the kernel.

This feature is implemented entirely in `src/interrupts/keyboard.c`. The allocation lifecycle is intentionally visible in the code:

- a command line is copied into heap memory with `malloc(length + 1)`
- the copied line is stored in the history buffer
- old entries are freed automatically when the fixed-capacity history fills
- `clearhistory` explicitly frees every stored entry

That means the feature is not just a cosmetic shell improvement. It is a direct runtime demonstration that `malloc()` and `free()` are being exercised by the kernel.

### 10.1 CLI Commands

The keyboard handler was gradually expanded from a simple input logger into a small CLI. The supported commands are:

- `help` : show the available commands
- `clear` : clear the terminal view
- `meminfo` : print heap and page-memory information
- `history` : show the saved command history
- `clearhistory` : free all stored history entries
- `ticks` : show the current PIT tick count
- `uptime` : show uptime in milliseconds
- `echo <text>` : print text back to the terminal
- `about` : print a short feature summary

These commands were added in `src/interrupts/keyboard.c` as part of the proof layer around the assignment features. The assignment itself did not require a CLI, but the CLI made the memory manager and PIT behavior easier to demonstrate and easier to explain in the report.

### 10.2 Scrollback and Keyboard Navigation

The original terminal improvement only added safe screen scrolling in the sense of preventing the cursor from running off the VGA buffer. It still discarded old text once the visible screen filled up. That was not ideal for a CLI, because command output such as `history`, `help`, or `meminfo` could move off-screen too quickly.

To solve that, the terminal in `src/libc/stdio.c` was extended with a software scrollback buffer and a viewport:

- output is written to an in-memory terminal buffer
- the visible 80x25 VGA area is re-rendered from that buffer
- the viewport can be moved independently of the live cursor

The keyboard handler in `src/interrupts/keyboard.c` now interprets selected extended scancodes so the user can navigate terminal history:

- `PgUp` and `PgDn` scroll by pages
- `Up` and `Down` scroll line by line
- `Home` jumps to the top of scrollback
- `End` jumps back to the live bottom

This turned the kernel terminal into something much closer to a real CLI environment instead of a one-way print log.

### 10.3 Cursor Synchronization and Display Limits

One later usability issue was that the visible typing position in QEMU did not always match where the kernel was actually writing text. The reason was that the terminal implementation in `src/libc/stdio.c` maintained its own software cursor (`terminal_row` and `terminal_column`), but did not synchronize that position with the VGA hardware cursor shown by the emulator.

The fix was implemented directly in `src/libc/stdio.c`:

- the VGA CRT controller ports `0x3D4` and `0x3D5` are now programmed explicitly
- the hardware cursor shape is enabled during normal terminal use
- the hardware cursor location is updated whenever the visible terminal view is rendered
- the cursor is hidden automatically when the user scrolls away from the live bottom and the active input position is outside the visible viewport

This made the CLI feel significantly more correct, because the visual cursor shown in QEMU now follows the actual write location used by the kernel.

During the same review, the video path itself was re-evaluated. The current terminal still writes directly to the VGA text-mode buffer at `0xB8000` in `src/libc/stdio.c`, and the optional Multiboot framebuffer request in `src/multiboot2.asm` is still commented out. That has an important consequence: the guest terminal is still fundamentally a fixed 80x25 text terminal.

Because of that design, two related observations were documented explicitly:

- making the QEMU window larger can make the guest output easier to see, but it does not give the guest more text columns or more text rows
- true dynamic resizing, where the terminal shows more text when the window becomes larger, would require moving away from plain VGA text mode to a framebuffer-based renderer

To still improve usability on the host side, `scripts/start_qemu.sh` was extended with optional fullscreen support. QEMU can now be started with either:

```bash
QEMU_FULLSCREEN=1 ./src/3_jakub/scripts/start_qemu.sh <kernel-image> <disk-image>
```

or:

```bash
./src/3_jakub/scripts/start_qemu.sh <kernel-image> <disk-image> --fullscreen
```

This is a presentation improvement for the emulator window, not a change to the guest terminal geometry. That distinction is important for the credibility of the report.

## 11. Build and Validation

Validation was done in several stages.

### 11.1 Build Validation

The kernel was rebuilt successfully with:

```bash
cmake --build build/3_jakub -j4
```

After that, a bootable image was created successfully with:

```bash
cmake --build build/3_jakub --target uiaos-create-image -j2
```

This confirmed that:

- the new files were correctly added to `CMakeLists.txt`
- the kernel still linked correctly
- the ISO generation flow still worked

### 11.2 Symbol Validation

The final kernel binary was inspected with `i686-elf-nm`. The following required symbols were present in `build/3_jakub/kernel.bin`:

- `init_kernel_memory`
- `init_paging`
- `print_memory_layout`
- `malloc`
- `free`
- `init_pit`
- `sleep_busy`
- `sleep_interrupt`
- `pit_get_ticks`
- `operator new`
- `operator delete`

This confirmed that the assignment functionality was part of the active binary and not just present in unused source files.

### 11.3 QEMU and GDB Validation

QEMU and `gdb-multiarch` were used to verify the runtime path.

The first debugger pass confirmed that execution reached:

- `init_paging()`
- `init_pit()`
- `kernel_main()`

The CPU registers also confirmed that paging was enabled:

- `cr0 = 0x80000011`
- `cr3 = 0x400000`

This showed that:

- the page directory was loaded into `cr3`
- the paging bit in `cr0` was enabled

The same debugger session also showed that the first VGA text cells at `0xB8000` were initialized to `0x0f20`, which is a blank white-on-black text cell. This confirmed that the display path was explicitly clearing the screen instead of leaving stale video memory behind.

The second debugger pass confirmed that execution reached:

- the line immediately after `sleep_busy(50)`
- the line immediately after `sleep_interrupt(50)`
- the C++ allocation test line in `kernel_main()`

At that point, `pit_get_ticks()` returned `102`, showing that timer ticks were advancing while the kernel was running.

Together, these checks provided strong evidence that:

- paging initializes correctly
- the PIT is active
- both sleep functions return
- the C++ allocation path is reached successfully

Later fixes were also validated in smaller targeted ways:

- the repeated page-fault issue was investigated by tracing the boot path through `_start` in `src/multiboot2.asm`
- the input regression after that fix was traced to interrupts being disabled in the idle loop
- the terminal clear path was validated by checking that VGA memory at `0xB8000` contained blank text cells after initialization

Because the sandbox environment is not a full interactive desktop session, the final end-to-end feel of the CLI and scrollback is still best confirmed by booting locally and trying the commands directly. The build, ISO generation, and code-path checks all succeeded, but the final user-experience validation is naturally strongest when tested interactively in QEMU.

## 12. Results

The Assignment 4 implementation is now integrated into the active `3_jakub` kernel build.

The final result includes:

- working kernel memory initialization
- working `malloc()` and `free()`
- working paging initialization
- working memory layout reporting
- working PIT initialization on IRQ0
- working busy-wait and interrupt-driven sleep functions
- working overloaded C++ `new` and `delete`
- a cleared and scroll-safe VGA terminal
- a heap-backed keyboard history feature for visible runtime proof of memory use
- a small interactive CLI for demonstrating the implemented features
- keyboard-controlled terminal scrollback for revisiting previous output
- synchronized VGA hardware cursor behavior in the CLI
- optional fullscreen launch support for the QEMU helper script

The code compiles, links, packages into an ISO, and passes debugger-based runtime validation for the main control flow.

## 13. Known Limitations

The implementation is intentionally simple and assignment-focused. The main limitations are:

- the heap allocator is still a basic first-fit allocator
- there is no advanced protection against invalid frees or heap corruption
- the page-aligned allocator uses a fixed number of slots
- paging is fixed to a simple identity-mapped first 8 MB
- the solution does not yet use a dynamic physical memory map from the bootloader
- the sleep timing is only as accurate as the PIT tick rate and scheduler-free kernel environment allow
- the history feature stores a limited number of entries and is intentionally simple
- the CLI is deliberately minimal and does not include full line editing, parsing, or command completion
- scrollback is implemented in software and controlled from keyboard shortcuts rather than mouse wheel support
- the visible terminal area is still fixed to VGA text mode, so enlarging the QEMU window does not create more guest-side text rows or columns
- mouse-wheel scrolling is not implemented because the kernel does not yet include a PS/2 mouse driver or wheel-packet handling path

These limitations are acceptable for the assignment because the main goal is to demonstrate working memory management and PIT-based sleeping rather than building a production-grade kernel memory subsystem.

## 14. Conclusion

Assignment 4 was completed by integrating a simple kernel memory manager and a PIT-based timer subsystem into the active `3_jakub` codebase. The final solution stayed close to the assignment requirements, while also adapting the provided templates to fit the real project structure and interrupt model already present in the kernel.

The result is a working foundation for future assignments. Memory allocation is now available in both C and C++, paging is enabled during boot, and the kernel can delay execution using either busy waiting or interrupt-driven sleeping. On top of that, the kernel now has enough CLI behavior to make those features observable in practice, which makes the final implementation easier both to demonstrate and to defend in a report.
