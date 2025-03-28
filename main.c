

// Created: 2025-03-06 16:00:00
// Author: Michael Schneider
// -----------------------------------------------------
// Target Device: STM32F411RE
// -----------------------------------------------------
// Programming Language: C, bare metal
//
// This is a test program for the STM32F411RE to explore
// the capabilities of the STM32F411RE microcontroller.
// The program is intended to create 
//    - an accurate base tick (10ms) (SYSTICK IRQ)
//    - GPIO output
//    - SPI communication
//    - CAN communication
// -----------------------------------------------------


// USART1 to be used with F411RE, USART2  to be used with F429ZI 
// ..

#include "startup.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "registers.h"
#include "serial.h"
#include "conversion.h"
#include "can.h"
#include "VEcan.h"
// #include "I2C.h"
#include "INA228.h"

#define FREQ 16000000  // PCLK, internal clock by default, 16 Mhz
#define BIT(x) (1UL << (x))

static volatile uint32_t pick = 0;


char charstr[] = "Hello World!\r\n";
char mesg1[] = "success, can started!\r\n";
char mesg2[] = "no success, can not started!\r\n";
char mesg3[] = "trying to start can bus ";
char mesg4[] = ".";
char mesg5[] = "Data received!\r\n";

uint32_t zonk = 0;

uint32_t zonkchar = 0;
char zonkachar;
bool kazuka = false;
bool canrx_pending = false;

char bla;

char txt[10];
uint32_t myNum = 3367756;
int lenght = 0;
int i = 0;
uint8_t lsb;
uint8_t msb;

uint8_t dataLsb;
uint8_t dataMsb;


uint32_t Sprong1, Sprong2;
uint64_t Kaponk;
uint64_t Kagong;


CAN_RX_FRAME canrx;
CAN_TX_FRAME cantx;

volatile uint32_t interrupt_count = 0;




static inline void systick_init(uint32_t ticks) {
  if ((ticks - 1) > 0xffffff) return;    // Systick timer is 24 bit
  SYST->RVR = ticks - 1;
  SYST->CVR = 0;
  SYST->CSR = BIT(0) | BIT(1) | BIT(2);  // Enable systick
  RCC->APB2ENR |= BIT(14);               // Enable SYSCFG
}

static inline void PA5_out_init() {
  RCC->AHB1ENR |= 1 << 0;             // Enable GPIOA clock
  GPIOA->MODER |= 1 << 10;            // Set PA5 to output mode
  GPIOA->ODR |= (1 << 5);             // Set PA5
}

static inline void PB7_out_init() {
  RCC->AHB1ENR |= 1 << 1;             // Enable GPIOB clock
  GPIOB->MODER |= 1 << 14;            // Set PB7 to output mode
  GPIOB->ODR |= (1 << 7);             // Set PB7
}


int main(void) {

  
  cantx.identifier = 0x446;
  cantx.length=0x0008U;
  cantx.data[0] = 0xFF;
  cantx.data[1] = 0xAF;
  cantx.data[2] = 0x0F;
  cantx.data[3] = 0xF1;
  cantx.data[4] = 0x22;
  cantx.data[5] = 0x22;
  cantx.data[6] = 0x11;
  cantx.data[7] = 0xBB;

  systick_init(FREQ / 1);   // 1s second (STM32F4 runs at 16MHz)
  //PB7_out_init();           // Set blue LED to output mode
  uart_init(UART2, 9600);   // Initialize UART2 with 9600 baud rate

  INA228_Init();
  if (INA228_ReadVBUS(&dataLsb, &dataMsb)) {
    lsb = dataLsb;
    msb = dataMsb;
  }





  


  Can_Init(CAN1);
  Can_Filter(CAN1, 0x307);
  if (Can_Start(CAN1)) {                // can start successfull
    lenght = lenghtofarray(mesg1);
    uart_write_buf(UART2, mesg1, lenght);
    i = 0;
  }
  else {       
    i = 0;
    lenght = lenghtofarray(mesg3);
    uart_write_buf(UART2, mesg3, lenght);   // can not started, trying again
    while (i<10) {
      if (!(Can_Start(CAN1))) {
        lenght = lenghtofarray(mesg4);
        uart_write_buf(UART2, mesg4, lenght);
        i++;
        if (i==10) {
          lenght = lenghtofarray(mesg2);
          uart_write_buf(UART2, mesg2, lenght);
        }
      }
      else {
        lenght = lenghtofarray(mesg1);
        uart_write_buf(UART2, mesg1, lenght);
        i = 10;
      }
  
    }

  }

  VECan_Init();

  while (1) {
  
    spin(999999);  // Delay
    Can_SendMessage(CAN1, &cantx);
    VECan_send();

    spin(99999);  // Delay

    if (kazuka) {
      uart_write_buf(UART2, &bla, 1);
      kazuka = false;
    }



    if (canrx_pending) {
      lenght = lenghtofarray(mesg5);
      uart_write_buf(UART2, mesg5, lenght);
    }
 
    GPIOB->ODR ^= (1 << 7);  // Toggle PB7
    spin(99999);  // Delay
      }
  return 0;
}


void SysTick_Handler(void) {
  //pick++;
  // GPIOA->ODR ^= (1 << 5);  // Toggle PA5
  GPIOB->ODR ^= (1 << 7);  // Toggle PB7
}

void USART1_IRQHandler(void) {
  if (UART1->SR & BIT(5)) {
    bla = (UART1->DR & 0xFF);   // Read data register and by reading, the interrupt flag is cleared
    uart_write_buf(UART1, &bla, 1);
    kazuka = true;
  }
}

void USART2_IRQHandler(void) {
  if (UART2->SR & BIT(5)) {
    bla = (UART2->DR & 0xFF);   // Read data register and by reading, the interrupt flag is cleared
    uart_write_buf(UART2, &bla, 1);
    kazuka = true;
  }
}



void CAN1_RX0_IRQHandler(void) {
  interrupt_count++;
  canrx_pending = true;
  Can_ReceiveMessage (CAN1, &canrx);
}

void CAN1_RX1_IRQHandler(void) {
  interrupt_count++;
  canrx_pending = true;
  Can_ReceiveMessage (CAN1, &canrx);
}



/*/ Startup code
__attribute__((naked, noreturn)) void _reset(void) {
  // memset .bss to zero, and copy .data section to RAM region
  extern long _sbss, _ebss, _sdata, _edata, _sidata;
  for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
  for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;

  main();             // Call main()
  for (;;) (void) 0;  // Infinite loop in the case if main() returns
}

extern void _estack(void);  // Defined in link.ld

// 16 standard and 91 STM32-specific handlers
__attribute__((section(".vectors"))) void (*const tab[16 + 91])(void) = {
  _estack, _reset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SysTick_Handler,
  // IRQ0 to IRQ36 (positions 16 to 52)
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // IRQ0 .. IRQ9
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // IRQ10 .. IRQ19
  CAN1_RX0_IRQHandler,          // IRQ20
  0, 0, 0, 0, 0, 0, 0, 0, 0,    // IRQ21 .. IRQ29
  0, 0, 0, 0, 0, 0, 0,          // IRQ30 .. IRQ36
  USART1_IRQHandler,  // IRQ37
  USART2_IRQHandler,  // IRQ38
  0,0,0};
*/
