
// Created: 2025-03-19 07:00:00
// Author: M. Schneider
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
int32_t counter_loop = 0;


// function for initializing the can Interface for STM32F439
// can_rx on PD0, can_tx on PD1

bool Can_Init(struct can *can) {

    RCC->AHB1ENR |= BIT(3);         // enable clock for GPIOD
    GPIOD->MODER |= BIT(1);         // set PD0 to "alternate" function
    GPIOD->MODER &= ~BIT(0);        
    GPIOD->MODER |= BIT(3);         // set PD1 to "alternate" function
    GPIOD->MODER &= ~BIT(2);
    GPIOD->AFRL |= (0x09 << 0)|(0x09 << 4); // set AF9 for pin 0 and pin 1
   
    RCC->APB1ENR |= BIT(25);        // enable the clock for can1
    
    can->MCR |= BIT(0);             // enable can initialization mode

    counter_loop = 0;
    while (!(can->MSR & BIT(0)))    // check if INAK bit is set *meaning HW is in init. --> check if syntax is ok.
    {                                   
        if (counter_loop > 1000) {
        return false;
        }
        counter_loop++;
    }; 



    can->MCR &= ~BIT(1);            // exit sleep mode

    counter_loop = 0;
    while (can->MSR & BIT(1))       // check SLAK bit is clear *meaning no sleep
    {                                   
        if (counter_loop > 1000) {
        return false;
        }
        counter_loop++;
    }; 


    /* configure the timing with following parameters
        * Normal Mode
        * Loop back mode is disabled
        * Resync jump width to 1 (value - 1).
        * Prescale of 8. (value - 1).
        * Time quanta segment 1 is 2 (value - 1).
        * Time quanta segment 2 is 1 (value - 1).
        * Baud rate is 500 Kbps
        * */

    can->BTR &= ~((0xFUL << 16)|(0x7UL << 20)|(0x3UL << 24)); // reset BTR to zero values
    can->BTR = (0x1UL << 16) | (0 << 20) | (7 << 0); // set BRP, TS1 and TS2
    
    // put temporary in loop back mode
    // can->BTR |= BIT(30);    // to be deleted!!

    // Enable CAN1_Receive FIFO-0 interrupt
    can->IER = 0;   // clear the register
    can->IER |= BIT(1);              // enable Receive Interrupt FMPIE0
    

    // Enable CAN1_RX0 interrupt in NVIC
    NVIC->ICER[0] = 0;
    NVIC->ISER[0] |= (1 << 20);         // Set bit 20 in NVIC_ISER0
    NVIC->IPR[20] = 0x80;               // set the interrupt priority
    
    return true;                // return success

    }


bool Can_Start (struct can *can) {
    // Leave Initialization Mode
    

    can->MCR &= ~BIT(1);            // Clear SLEEP
    can->MCR &= ~BIT(0);            // Clear INRQ to go to "normal mode"
    
    for (int i=0; i<100; i++)      // wait a little 
    {
        asm("nop");
    }

    if (can->MSR & BIT(0)) {        // Check INAK (if clear then ini finished)
        return (0);                 // return no success
    }      
    else return (1);                // return success
    }

    

void Can_Filter (struct can *can, uint16_t identifier) {
    can->FMR |= BIT(0);             // set FINIT, Filter init mode on
    can->FMR &= ~(0x00003F00U);     // Slave Filter to start from 20
    can->FMR |= (20<< 8);           // CAN1 Filter is assigned from 0 to 19.
    
    can->FA1R &= ~(BIT(18));        // deactivate filter 18 (clear FACT bit)
    can->FS1R |= BIT(18);           // Set for single 32bit scale config for filter 18 (set FSC18)
    can->FM1R &= ~BIT(18);          // Set to 1 for identifier "mask" mode
                                    // FSC18 = 1, FBM18 = 0 *mapping see Figure 342
    
    can->F18R1 = 0;
    can->F18R2 = 0;

    can->F18R1 = (identifier << 5) << 16;    // Identifier *shift the ID (11bit) to the most left.
    can->F18R2 = (identifier << 5) << 16;    // Identifier Mask

    can->FFA1R &= ~BIT(18);         // Filter 18 assigned to FIFO-0
    can->FA1R |= BIT(18);           // activate Filter 18

    can->FMR &= ~BIT(0);            // clear the filter init mode

}

bool Can_SendMessage (struct can *can, CAN_TX_FRAME *TXFrame) {
    counter_loop = 0;
    while ((can->TSR & (0x1UL << 26U)) == 0)    // wait until transmit mailbox 0 is empty *TME0->Bit26
    {                                   
        if (counter_loop > 1000) {
        return false;
        }
        counter_loop++;
    }; 

    can->TI0R &= ~(0x7FFUL << 21U);             // set "standart" identifier
    can->TI0R &= ~BIT(1);                       // set Data Frame
    can->TI0R |= (TXFrame->identifier << 21);   // set the identifier
    can->TDT0R &= ~(BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(8)); // clear data length.
    can->TDT0R |= TXFrame->length << 0;         // set the data length
    

    // store the data into the mailbox LOW
    can->TDL0R = 
        ((uint32_t) TXFrame->data[3] << 24U) |
        ((uint32_t) TXFrame->data[2] << 16U) |
        ((uint32_t) TXFrame->data[1] << 8U) |
        ((uint32_t) TXFrame->data[0] << 0U);  
    // store the data into the mailbox HIGH
    can->TDH0R = 
        ((uint32_t) TXFrame->data[7] << 24U) |
        ((uint32_t) TXFrame->data[6] << 16U) |
        ((uint32_t) TXFrame->data[5] << 8U) |
        ((uint32_t) TXFrame->data[4] << 0U);  
    
    // start the transmission
    can->TI0R |= (1U << 0U);        // transmission request TXRQ

    return true;                // return success
    
}

bool Can_ReceiveMessage (struct can *can, CAN_RX_FRAME *RXFrame) {
    if (can->RF0R & 0x00000003U)            // If FIFO-0 has pending message
        {      
            RXFrame->identifier = (can->RI0R >> 3) & 0x1FFFFFFF;    // get identifier of data
            RXFrame->length = can->RDT0R & 0x0F;                    // get legth of data
            
            for (int i=0; i<8; i++)         // clear the old data before getting new one from FIFO
            {
                RXFrame->data[0]=0;
            }
        
        RXFrame->data[0]=(uint8_t)(can->RDL0R >> 0U);
        RXFrame->data[1]=(uint8_t)(can->RDL0R >> 8U);
        RXFrame->data[2]=(uint8_t)(can->RDL0R >> 16U);
        RXFrame->data[3]=(uint8_t)(can->RDL0R >> 24U);
        RXFrame->data[4]=(uint8_t)(can->RDH0R >> 0U);
        RXFrame->data[5]=(uint8_t)(can->RDH0R >> 8U);
        RXFrame->data[6]=(uint8_t)(can->RDH0R >> 16U);
        RXFrame->data[7]=(uint8_t)(can->RDH0R >> 24U);

        can->RF0R |= BIT(5);        // Release the FIFO by setting RFOM0
        
        return (1);
        }
    else return (0);
}


