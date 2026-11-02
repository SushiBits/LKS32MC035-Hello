/**
 * @file system_LKS32MC03x.h
 */

#ifndef _SYSTEM_LKS32MC03x_H_
#define _SYSTEM_LKS32MC03x_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

extern uint32_t SystemCoreClock;
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);

__END_DECLS

#endif // defined(_SYSTEM_LKS32MC03x_H_)