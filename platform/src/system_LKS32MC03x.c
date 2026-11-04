
#include "LKS32MC03x.h"

uint32_t SystemCoreClock = 4000000;

#ifndef LSI_VALUE
#define LSI_VALUE 64000
#endif

#ifndef HSI_VALUE
#define HSI_VALUE 4000000
#endif

#ifndef PLL_VALUE
#define PLL_VALUE 48000000
#endif


__attribute__((weak)) void SystemInit(void) {
    // Unlock SYS.
    SYS->PROTECT = SYS_PROTECT_PSW_PSW;

    // 48MHz clock.
    // This chip does not have PLL state read-backs, so we simply blind trust the clock configuration.
    SYS->CLK_CFG =
        (SYS_CLK_CFG_CLK_SEL_PLL << SYS_CLK_CFG_CLK_SEL_Pos) |
        (SYS_CLK_CFG_CLK_DIV_DIV1 << SYS_CLK_CFG_CLK_DIV_Pos);
}

void SystemCoreClockUpdate(void) {
    // Extract components of the clock configuration.
    unsigned CLKSRC = (SYS->CLK_CFG & SYS_CLK_CFG_CLK_SEL_Msk) >> SYS_CLK_CFG_CLK_SEL_Pos;
    unsigned PLLDIV = (SYS->CLK_CFG & SYS_CLK_CFG_CLK_DIV_Msk) >> SYS_CLK_CFG_CLK_DIV_Pos;

    switch (CLKSRC) {
        case SYS_CLK_CFG_CLK_SEL_HRC: // HRC clock
            SystemCoreClock = HSI_VALUE;
            break;
        case SYS_CLK_CFG_CLK_SEL_PLL: // PLL clock
            // NB: The datasheet only specified PLL out frequency at some specific PLLDIV values.
            // I suspect a formula is possible but testing needed with an oscilloscope.
            if (PLLDIV <= 0x01) SystemCoreClock = PLL_VALUE / 8;
            else if (PLLDIV <= SYS_CLK_CFG_CLK_DIV_DIV4) SystemCoreClock = PLL_VALUE / 4;
            else if (PLLDIV <= SYS_CLK_CFG_CLK_DIV_DIV2) SystemCoreClock = PLL_VALUE / 2;
            else SystemCoreClock = PLL_VALUE;
            break;
        default: // LRC clock
            SystemCoreClock = LSI_VALUE;
    }
}
