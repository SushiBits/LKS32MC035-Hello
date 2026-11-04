
#include <LKS32MC03x.h>

int main(void) {
    // Disable WDT.
    // The IWDG is enabled by default with or without a debugger attached.
    // We are debugging code now, which does not need WDT (it can be counterproductive at times) so we disable
    // this for now.
    // This line disables IWDG raising wakeups and resets at the same time. We don't use deep sleeps here so
    // disabling wakeups are okay. (IMHO this is a motor driver with one giant current hog connected to our chip,
    // pardon my lack of understanding of the purpose of squeezing ultra low current here. SysTick alone should
    // suffice, along with nice Arduino-ish millis() and delay() functions. But I digress.)
    // p.s. This harkens of the TI MSP430 CPU16 series. Haven't tried MSPM0 yet but I suspect it is the same.
    IWDG->CFG = IWDG_CFG_DISABLE_KEY_KEY << IWDG_CFG_DISABLE_KEY_Pos;

    for (;;) __WFE(); // Idle loop. We use WFE instead of WFI because we want to be able to wake up from interrupts.
}