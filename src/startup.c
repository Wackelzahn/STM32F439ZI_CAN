
// startup.c
#include <stdint.h>

// External symbols defined in link.ld or other files
extern long _sbss, _ebss, _sdata, _edata, _sidata;
extern void _estack(void);  // Stack top, defined in link.ld
extern int main(void);      // Main function from main.c

// Weak declarations for interrupt handlers (can be overridden elsewhere)
void SysTick_Handler(void) __attribute__((weak));
void CAN1_RX0_IRQHandler(void) __attribute__((weak));
void USART1_IRQHandler(void) __attribute__((weak));
void USART2_IRQHandler(void) __attribute__((weak));

// Reset handler: Initialize .bss and .data, then call main()
__attribute__((naked, noreturn)) void _reset(void) {
  // Zero out .bss section
  for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
  // Copy .data section from flash to RAM
  for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;

  main();             // Call main()
  for (;;) (void) 0;  // Infinite loop if main() returns
}

// Interrupt vector table
__attribute__((section(".vectors"))) void (*const tab[16 + 91])(void) = {
    _estack,           // 0: Stack pointer
    _reset,            // 1: Reset handler
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 2-14: Core exceptions
    SysTick_Handler,   // 15: SysTick
    // IRQ0 to IRQ36 (positions 16 to 52)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // IRQ0 .. IRQ9
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // IRQ10 .. IRQ19
    CAN1_RX0_IRQHandler,   // IRQ20: CAN1 RX0
    0, 0, 0, 0, 0, 0, 0, 0, 0,    // IRQ21 .. IRQ29
    0, 0, 0, 0, 0, 0, 0,          // IRQ30 .. IRQ36
    USART1_IRQHandler,     // IRQ37: USART1
    USART2_IRQHandler,     // IRQ38: USART2
    0, 0, 0               // IRQ39 .. IRQ41 (and beyond, up to 106 total)
};

// Default weak implementations (optional, can be overridden in other files)
__attribute__((weak)) void SysTick_Handler(void) { while (1); }
__attribute__((weak)) void CAN1_RX0_IRQHandler(void) { while (1); }
__attribute__((weak)) void USART1_IRQHandler(void) { while (1); }
__attribute__((weak)) void USART2_IRQHandler(void) { while (1); }