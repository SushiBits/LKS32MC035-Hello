/**
 * @file main.c
 * 
 * Main entry point for the LKS32MC03x microcontroller.
 */

#include "LKS32MC03x.h"

#include "gpio.h"

int main(void) {
    // Disable WDT.
    // The IWDG is enabled by default with or without a debugger attached.
    // We are debugging code now, which does not need WDT (it can be counterproductive at times) so we disable
    // this for now. For production code, we should enable this later after peripheral initialization and before
    // entering the idle loop.
    // The straight assignment is important here. Disabling of the IWDG (writing 0 to WDG_EN) must occur in the same
    // write as the key. Otherwise, the IWDG will not be disabled. This is a hardware design choice by the chip
    // manufacturer, as indicated in the reference manual. Changing this assignment to |= with IWDG enabled (power-on
    // default) will not disable the IWDG, and the chip will reset after a few seconds.
    // This line disables IWDG raising wakeups and resets at the same time. We don't use deep sleeps here so
    // disabling wakeups are okay. (IMHO this is a motor driver with one giant current hog connected to our chip,
    // pardon my lack of understanding of the purpose of squeezing ultra low current here. SysTick alone should
    // suffice, along with nice Arduino-ish millis() and delay() functions. But I digress.)
    // p.s. This harkens of the TI MSP430 CPU16 series. Haven't tried MSPM0 yet but I suspect it is the same.
    IWDG->CFG = IWDG_CFG_DISABLE_KEY_KEY << IWDG_CFG_DISABLE_KEY_Pos;

    // Prepare GPIO.
    gpio_init();

    for (;;) __WFE(); // Idle loop.
}