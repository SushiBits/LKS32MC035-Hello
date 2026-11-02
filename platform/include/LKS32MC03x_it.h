/**
 * @file LKS32MC03x_it.h
 * 
 * Interrupt vector for LKS32MC03x microcontroller.
 */

#ifndef _LKS32MC03x_IT_H_
#define _LKS32MC03x_IT_H_

#include <sys/cdefs.h>

typedef void (*vector_t)(void);

__BEGIN_DECLS

extern vector_t vector[];

#define IRQN_HANDLER(n, x) void x##_IRQHandler(void);
#define SKIP_HANDLER(n)
#include "LKS32MC03x_handlers.h"
#undef IRQN_HANDLER
#undef SKIP_HANDLER

__END_DECLS

#endif // defined(_LKS32MC03x_IT_H_)
