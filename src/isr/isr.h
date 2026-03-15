#ifndef ISR_H
#define ISR_H

#include "idt.h"

struct interrupt_frame;

__attribute__((interrupt)) void interrupt_handler(struct interrupt_frame* frame);

__attribute__((interrupt)) void isr0(struct interrupt_frame* frame); //divide by zero
__attribute__((interrupt)) void isr1(struct interrupt_frame* frame); //debug interrupt
__attribute__((interrupt)) void isr14(struct interrupt_frame* frame); //page fault


#endif