
// Created: 2025-03-19 07:00:00
// Author: Michael Schneider
// -----------------------------------------------------
// Target Device: STM32FF439ZI
// -----------------------------------------------------
// Programming Language: C, bare metal
//
// This is the implementation file CAN bus interfacing 
// for the STM32F439
// This source code is intended to create 
//    - the initialization of the can interface
//      1. Initialization of the GPIO
//      2. 
// -----------------------------------------------------




#include "can.h"
#include "registers.h"

#define BIT(x) (1UL << (x))



// function for initializing the can Interface for STM32F439
// can_rx on PD0, can_tx on PD1

void Can_Init(struct can *can) {

    RCC->AHB1ENR |= BIT(3);         // enable clock for GPIOD
    GPIOD->MODER |= BIT(1);         // set PD0 to "alternate" function
    GPIOD->MODER &= ~BIT(0);        
    GPIOD->MODER |= BIT(3);         // set PD1 to "alternate" function
    GPIOD->MODER &= ~BIT(2);
    GPIOD->AFRL |= (0x09 << 0)|(0x09 << 1); // set AF9 for pin 0 and pin 1
    
    RCC->APB1ENR |= BIT(25);        // enable the clock for can1
    can->MCR |= BIT(0);            // enable can initialization mode
    while (!(can->MSR & BIT(0)));  // check if INAK bit is set *meaning HW is in init. --> check if syntax is ok.
    can->MCR &= ~BIT(1);           // exit sleep mode
    while ((can->MSR & BIT(1))!=0U);   // check SLAK bit is clear *meaning no sleep

    /* configure the timing with following parameters
        * Normal Mode
        * Loop back mode is disabled
        * Resync jump width to 1 (value - 1).
        * Prescale of 8. (value - 1).
        * Time quanta segment 1 is 2 (value - 1).
        * Time quanta segment 2 is 1 (value - 1).
        * Baud rate is 500 Kbps
        * */

    can->BTR &= ~((0xFU << 16)|(0x7U << 20)|(0x1FFU << 0)); // reset BTR to zero values
    can->BTR = (1U << 16) | (0U << 20) | (7U << 0); // set BRP, TS1 and TS2

    }

void Can_Start (struct can *can) {
    // Leave Initialization Mode
    can->MCR &= ~BIT(0);            // Clear 
    while (can->MSR & BIT(0));      // Check INAK (if clear then ini finished)
}