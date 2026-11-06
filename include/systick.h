/**
 * @file systick.h
 * 
 * Arduino-style time and delay functions.
 *
 * Everything here is derived from SysTick, so systick_init() has to run once
 * before any of it is useful. The reload value comes from SystemCoreClock, so
 * that has to be correct first; Reset_IRQHandler() calls
 * SystemCoreClockUpdate() before main() runs, so it is.
 */

#ifndef _SYSTICK_H_
#define _SYSTICK_H_

#include "LKS32MC03x.h"
#include <sys/cdefs.h>

__BEGIN_DECLS

/**
 * @brief Start the 1ms system tick.
 *
 * Configures SysTick to interrupt once per millisecond and clears the
 * millisecond counter.
 *
 * @note Calling this again while a delay is in progress restarts the timebase
 *       underneath it, and resets millis() to zero.
 * @note Requires SystemCoreClock to hold the real core frequency, and to be at
 *       least 1kHz or the reload value truncates to zero and no tick is ever
 *       generated.
 */
void systick_init(void);

/**
 * @brief Milliseconds elapsed since systick_init().
 *
 * @return Whole milliseconds, wrapping to zero after roughly 49.7 days.
 *
 * @note Compare timestamps by subtraction, never by magnitude, so the wrap is
 *       harmless: (uint32_t)(millis() - start) < period
 */
uint32_t millis(void);

/**
 * @brief Microseconds elapsed since systick_init().
 *
 * The millisecond counter alone is too coarse for anything timing a gate
 * driver, so this adds the fraction of the current tick taken from the SysTick
 * down-counter.
 *
 * @return Microseconds, wrapping to zero after roughly 71.6 minutes.
 *
 * @note The millisecond count and the cycle count are read with interrupts
 *       masked for a few cycles. Cortex-M0 has no exclusive access
 *       instructions, and without masking the tick can land between the two
 *       reads and the halves disagree by a full millisecond.
 * @note Below 1MHz a SysTick cycle is longer than a microsecond, so there is no
 *       sub-millisecond resolution to report and this degrades to millis() *
 *       1000.
 */
uint32_t micros(void);

/**
 * @brief Wait for the given number of milliseconds.
 *
 * @param ms Delay length in milliseconds. Zero returns immediately.
 *
 * @warning Waits with __WFE(). That only works if SysTick keeps counting while
 *          the core is asleep, and SysTick is clocked from the processor clock,
 *          which Sleep stops. If the core parks here and never returns, that is
 *          the reason: it is waiting for a tick that cannot arrive.
 */
void delay(uint32_t ms);

/**
 * @brief Wait for the given number of microseconds.
 *
 * @param us Delay length in microseconds.
 *
 * Whole milliseconds are spent in delay(); the remainder is timed on the
 * SysTick down-counter, so sub-millisecond delays are accurate to a single
 * cycle instead of rounding up to the next tick.
 *
 * @note Rounds up, never down. A delay is never shorter than requested, which
 *       is the safe direction for anything driving a power stage.
 * @note The millisecond component inherits the sleep caveat documented on
 *       delay().
 */
void delay_us(uint32_t us);

__END_DECLS

#endif // defined(_SYSTICK_H_)