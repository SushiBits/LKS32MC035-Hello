
#include <LKS32MC03x.h>

int main(void) {
    // Disable WDT.
    // The IWDG is enabled by default with or without a debugger attached.
    // We are debugging code now, which does not need WDT (it can be counterproductive at times) so we disable
    // this for now.
    // p.s. This harkens of the TI MSP430 CPU16 series. Haven't tried MSPM0 yet but I suspect it is the same.
    IWDG->CFG = 0x3c << 8;

    for (;;) __WFE(); // Idle loop. We use WFE instead of WFI because we want to be able to wake up from interrupts.
}