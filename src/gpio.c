/**
 * @file gpio.h
 * 
 * GPIO interface for the LKS32MC03x microcontroller.
 */

#include "gpio.h"
#include "LKS32MC03x.h"

void gpio_init(void) {
    // Strangely, this chip does not need to enable a clock for GPIO.

    // According to the schematics, we need to enable the following GPIO pins for the motor driver:
    // ZU, ZV, ZW = P0.7, P1.4 and P0.8 (zero-crossing comparators pin-change interrupts)
    // These pins are inputs reading from the CBM2091 high-voltage comparators. I left out the OD
    // pullups on the PCB to save myself some BOM lines, so the internal pullup need to be on.
    GPIO0->PUE |= (0b1 << 7) | (0b1 << 8);
    GPIO0->PIE |= (0b1 << 7) | (0b1 << 8);
    GPIO1->PUE |= (0b1 << 4);
    GPIO1->PIE |= (0b1 << 4);

    // LKS32MC03x have internal filter for GPIO, but we are filtering at the comparator here, so we
    // leave the GPIO filter unused.

    // These zero-crossing pins generate interrupts on all their transition edges.
    // TODO: Rename the EXTI fields in SVD. Their current names are plain wrong, referencing wirings of a
    // different chip.
    EXTI->CR0 |= (EXTI_CR0_T5_BOTH << EXTI_CR0_T5_Pos) | (EXTI_CR0_T6_BOTH << EXTI_CR0_T6_Pos);
    EXTI->CR1 |= (EXTI_CR1_T10_BOTH << EXTI_CR1_T10_Pos);

    // HU, HV, HW = P0.15, P0.14 and P0.13 (MCPWM outputs driving the gate drivers)
    // LU, LV, LW = P0.12, P0.11 and P0.10 (MCPWM outputs driving the gate drivers)
    // We need to be extra careful as not to create a glitch when handling those pins. They go direct
    // through the gate drivers into the power transistors. A glitch here can result in a dead short
    // which will pop a fuse and/or a power transistor.
    // We are using 3P3N drivers so the high side transistors need to stay HIGH while low side transistors
    // need to stay LOW here.
    MODIFY_REG(GPIO0->PDO, 0b111111 << 10, 0b111000 << 10);
    GPIO0->POE |= 0b111111 << 10;

    // We need to make sure the MCPWM is in its REMAP status before attaching the GPIO pins to it.
    SYS->CLK_FEN |= SYS_CLK_FEN_MCPWM_CLK_EN_ENABLED << SYS_CLK_FEN_MCPWM_CLK_EN_Pos;
    MCPWM->SWAP = MCPWM_SWAP_SWAP_SWAP; // Another magic in RM not in SVD. TODO: Fix SVD later to symbolize this.
    MODIFY_REG(GPIO0->AFR[2], 0xFF00, 0x3300);
    GPIO0->AFR[3] = 0x3333;
}