/**
 * @file systick.c
 */

#include "systick.h"
#include "LKS32MC03x_it.h"

static volatile uint32_t ms_counter;

void systick_init(void) {
    ms_counter = 0;
    SysTick_Config(SystemCoreClock / 1000);
}

void SysTick_IRQHandler(void) {
    ms_counter++;
}

uint32_t millis(void) {
    return ms_counter;
}

uint32_t micros(void) {
    // Cortex-M0 has no exclusive access instructions, so the two halves of the
    // timestamp have to be read with the tick masked out. Otherwise the ISR can
    // land between them and the milliseconds and cycles disagree by a whole ms.
    uint32_t primask = __get_PRIMASK();
    __set_PRIMASK(1);
    uint32_t ms = ms_counter;
    uint32_t elapsed = SysTick->LOAD - SysTick->VAL;
    __set_PRIMASK(primask);

    uint32_t ticks_per_us = SystemCoreClock / 1000000u;
    if (ticks_per_us == 0u) {
        // Below 1MHz a SysTick cycle is longer than a microsecond, so there is
        // no sub-millisecond resolution to report.
        return ms * 1000u;
    }

    return ms * 1000u + elapsed / ticks_per_us;
}

void delay(uint32_t ms) {
    uint32_t start = millis();

    // Unsigned arithmetic, so the millisecond counter wrapping is harmless.
    // Sleeping is only correct if SysTick keeps counting while the core is
    // asleep. It is clocked from the processor clock, and Sleep gates that
    // clock, so if this hangs on the bench the reason is that the core has
    // parked waiting for a tick that can no longer arrive.
    while ((millis() - start) < ms) {
        __WFE();
    }
}

void delay_us(uint32_t us) {
    // Whole milliseconds are burnt on the millisecond counter, which keeps long
    // delays independent of how fast this loop runs. The remainder is spent on
    // the cycle counter, so short delays are accurate to a cycle.
    if (us >= 1000u) {
        delay(us / 1000u);
        us %= 1000u;
    }

    if (us == 0u) return;

    uint32_t ticks_per_us = SystemCoreClock / 1000000u;
    uint32_t load = SysTick->LOAD;
    if (load == 0u || ticks_per_us == 0u) {
        // SysTick is not running, so there is no reference to wait against.
        // systick_init() needs to have been called.
        return;
    }

    // A remainder below 1000us is always shorter than one reload period, so the
    // counter can wrap at most once and a single comparison handles it.
    //
    // No WFE here on purpose, independent of whether SysTick survives Sleep: if
    // the only wake source is the 1ms tick, sleeping would round a 5us delay up
    // to a millisecond. This loop stays hot and accurate to a cycle.
    uint32_t target = us * ticks_per_us;
    uint32_t start = SysTick->VAL;

    for (;;) {
        uint32_t now = SysTick->VAL;
        uint32_t elapsed = (now <= start) ? (start - now) : (load + 1u - now + start);
        if (elapsed >= target) return;
    }
}
