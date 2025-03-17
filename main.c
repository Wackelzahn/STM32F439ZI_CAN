

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

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "registers.h"
#include "serial.h"
#include "conversion.h"

#define FREQ 16000000  // CPU frequency, 8 Mhz
#define BIT(x) (1UL << (x))

static volatile uint32_t pick = 0;


char charstr[] = "Hello World!\r\n";
uint32_t zonk = 0;

uint32_t zonkchar = 0;
char zonkachar;
bool kazuka = false;

char bla;

char txt[10];
uint32_t myNum = 3367756;
int lenght = 0;



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
  
  systick_init(FREQ / 1);  // 1s second (STM32F4 runs at 8MHz)
  // PA5_out_init();  // Set blue LED to output mode
  PB7_out_init();  // Set blue LED to output mode
  // uart_init(UART1, 9600);  // Initialize UART1 with 9600 baud rate
  uart_init(UART2, 9600);  // Initialize UART2 with 9600 baud rate
  
  while (1) {

    lenght = int2char(myNum, txt);
    //uart_write_buf(UART1, txt, lenght);
    spin(999999);  // Delay

   // if (uart_read_ready(UART1)) {
    //  char c = uart_read_byte(UART1);
    //  uart_write_buf(UART1, &c, 1);
   // }

    if (kazuka) {
      //uart_write_buf(UART1, &bla, 1);
      uart_write_buf(UART2, &bla, 1);
      kazuka = false;
    }

    //uart_write_buf(UART1, "\r\n", 2);  // Write message to UART1
    //lenght = lenghtofarray(charstr);
    //uart_write_buf(UART1, charstr, lenght);  // Write message to UART1
    //uart_write_buf(UART1, "hi\r\n", 4);  // Write message to UART1
    // GPIOA->ODR ^= (1 << 5);  // Toggle PA5
    GPIOB->ODR ^= (1 << 7);  // Toggle PB7
    spin(99999);  // Delay
      }
  return 0;
}

// Startup code
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
  _estack, _reset, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SysTick_Handler, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, USART1_IRQHandler, USART2_IRQHandler, 0,};

