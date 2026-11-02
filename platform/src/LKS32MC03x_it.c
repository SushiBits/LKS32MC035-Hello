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

static void Reset_IRQHandler(void) {
    for (;;) __WFE(); // stub
}

static void Default_IRQHandler(void) {
    __BKPT();
    for (;;) __WFE();
}
