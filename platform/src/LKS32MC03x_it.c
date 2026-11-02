/**
 * @file LKS32MC03x_it.c
 * 
 * Startup code in C for Cortex-M microcontroller
 */

#include "LKS32MC03x_it.h"

#include "LKS32MC03x.h"
#include <string.h>

void __stack(void); // Stack pointer, to be palced by linker script.
void _start(void); // C runtime entry point.
static void Reset_IRQHandler(void);
static void Default_IRQHandler(void);

__attribute__((used, section(".vector"))) vector_t vector[] = {
    [0] = __stack,
    [1] = Reset_IRQHandler,
#define IRQN_HANDLER(n, x) [n + 16] = x##_IRQHandler,
#define SKIP_HANDLER(n) [n + 16] = Default_IRQHandler,
#include "LKS32MC03x_handlers.h"
#undef IRQN_HANDLER
#undef SKIP_HANDLER
};

#define IRQN_HANDLER(n, x) __attribute__((weak, alias("Default_IRQHandler"))) void x##_IRQHandler(void);
#define SKIP_HANDLER(n)
#include "LKS32MC03x_handlers.h"
#undef IRQN_HANDLER
#undef SKIP_HANDLER

extern uint32_t __vector_start__;
extern uint32_t __vector_load__;
extern uint32_t __vector_end__;

extern uint32_t __data_start__;
extern uint32_t __data_load__;
extern uint32_t __data_end__;

extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

static void Reset_IRQHandler(void) {

    SystemInit(); // Initialize system clock

    memcpy(&__vector_start__, &__vector_load__, &__vector_end__ - &__vector_start__);
    memcpy(&__data_start__, &__data_load__, &__data_end__ - &__data_start__);
    memset(&__bss_start__, 0, &__bss_end__ - &__bss_start__);

    *((volatile uint32_t *)&SCB->RESERVED0 /* VTOR */) = (uint32_t)vector;

    _start();

    for (;;) __WFE();
}

static void Default_IRQHandler(void) {
    __BKPT();
    for (;;) __WFE();
}

__attribute__((weak, alias("Default_IRQHandler"))) void _exit(void);
