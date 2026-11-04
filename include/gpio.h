/**
 * @file gpio.h
 * 
 * Header for GPIO manipulation on the LKS32MC03x microcontroller.
 */

#ifndef _GPIO_H_
#define _GPIO_H_

#include <sys/cdefs.h>

#include "LKS32MC03x.h"

__BEGIN_DECLS

void gpio_init(void);

__END_DECLS

#endif // defined(_GPIO_H_)