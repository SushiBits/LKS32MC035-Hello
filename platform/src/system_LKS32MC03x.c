
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
    SYS->CLK_CFG =
        (SYS_CLK_CFG_CLK_SEL_PLL << SYS_CLK_CFG_CLK_SEL_Pos) |
        (SYS_CLK_CFG_CLK_DIV_DIV1 << SYS_CLK_CFG_CLK_DIV_Pos);
    SYS->DBG_CFG |= SYS_DBG_CFG_DBG_SLP_Msk | SYS_DBG_CFG_DBG_STOP_Msk;
}

void SystemCoreClockUpdate(void) {
    unsigned CLKSRC = (SYS->CLK_CFG & SYS_CLK_CFG_CLK_SEL_Msk) >> SYS_CLK_CFG_CLK_SEL_Pos;
    unsigned PLLDIV = (SYS->CLK_CFG & SYS_CLK_CFG_CLK_DIV_Msk) >> SYS_CLK_CFG_CLK_DIV_Pos;

    switch (CLKSRC) {
        case SYS_CLK_CFG_CLK_SEL_HRC: // HRC clock
            SystemCoreClock = HSI_VALUE;
            break;
        case SYS_CLK_CFG_CLK_SEL_PLL: // PLL clock
            if (PLLDIV <= 0x01) SystemCoreClock = PLL_VALUE / 8;
            else if (PLLDIV <= SYS_CLK_CFG_CLK_DIV_DIV4) SystemCoreClock = PLL_VALUE / 4;
            else if (PLLDIV <= SYS_CLK_CFG_CLK_DIV_DIV2) SystemCoreClock = PLL_VALUE / 2;
            else SystemCoreClock = PLL_VALUE;
            break;
        default: // LRC clock
            SystemCoreClock = LSI_VALUE;
    }
}
