
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
    SYS->PROTECT = 0x7a83;

    // 48MHz clock.
    // This chip does not have PLL state read-backs, so we simply blind trust the clock configuration.
    SYS->CLK_CFG =
        (SYS_CLK_CFG_CLK_SEL_PLL << SYS_CLK_CFG_CLK_SEL_Pos) |
        (SYS_CLK_CFG_CLK_DIV_DIV1 << SYS_CLK_CFG_CLK_DIV_Pos);

    // Allow debug in sleep and stop modes.
    // Also we halt the IWDG when debugging, so that we can debug without the IWDG resetting the device.
    // To be honest, it is better to set SYS_DBG_CFG_DBG_IWDG_STOP_ENABLED in a J-Link script file instead
    // of in code, but this is a good fallback for when the debugger does not set this bit.
    // Not quite part of this project, but since I am also maintaining the Linko devices' J-Link device definitions,
    // this should be a work item on my end.
    SYS->DBG_CFG |=
        SYS_DBG_CFG_DBG_SLP_ENABLED |
        SYS_DBG_CFG_DBG_STOP_ENABLED |
        SYS_DBG_CFG_DBG_IWDG_STOP_ENABLED;

    // Disable WDT.
    // The IWDG is enabled by default with or without a debugger attached.
    // Without this, the IWDG will reset the device amid a debugging session.
    // In this state only a system reset can get the device to connect to a debugger again.
    // So we disable the IWDG here to allow debugging.
    // p.s. This harkens of the TI MSP430 CPU16 series. Haven't tried MSPM0 yet but I suspect it is the same.
    IWDG->CFG = 0x3c << 8;
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
