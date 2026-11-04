/**
 * @file gpio.h
 * 
 * GPIO interface for the LKS32MC03x microcontroller.
 */

#include "gpio.h"

void gpio_init(void) {
    // Strangely, this chip does not need to enable a clock for GPIO.

    // According to the schematics, we need to enable the following GPIO pins for the motor driver:
    // ZU, ZV, ZW = P0.7, P1.4 and P0.8 (zero-crossing comparators pin-change interrupts pumping the 6-step state machine)
    // HU, HV, HW = P0.15, P0.14 and P0.13 (MCPWM outputs driving the gate drivers)
    // LU, LV, LW = P0.12, P0.11 and P0.10 (MCPWM outputs driving the gate drivers)
}