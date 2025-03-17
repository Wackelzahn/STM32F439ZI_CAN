#ifndef REGISTERS_H
#define REGISTERS_H

#include <inttypes.h>



// register RCC (Reset and Clock Control)
struct rcc {
    volatile uint32_t CR, PLLCFGR, CFGR, CIR, AHB1RSTR, AHB2RSTR, AHB3RSTR,
        RESERVED0, APB1RSTR, APB2RSTR, RESERVED1[2], AHB1ENR, AHB2ENR, AHB3ENR,
        RESERVED2, APB1ENR, APB2ENR, RESERVED3[2], AHB1LPENR, AHB2LPENR,
        AHB3LPENR, RESERVED4, APB1LPENR, APB2LPENR, RESERVED5[2], BDCR, CSR,
        RESERVED6[2], SSCGR, PLLI2SCFGR;
  };
  #define RCC ((struct rcc *) 0x40023800)


// register SYSTICK
  struct systick {
    volatile uint32_t CSR, RVR, CVR, CALIB;
  };
  #define SYST ((struct systick *) 0xe000e010)  // 2.2.2

// register NVIC
  struct nvic {
    volatile uint32_t ISER[8], RESERVED0[24], ICER[8], RESERVED1[24], ISPR[8],
        RESERVED2[24], ICPR[8], RESERVED3[24], IABR[8], RESERVED4[56], IPR[60];
  };
  #define NVIC ((struct nvic *) 0xe000e100)  // 4.3.1



// UART1 register
  struct uart {
    volatile uint32_t SR, DR, BRR, CR1, CR2, CR3, GTPR;
  };
  #define UART1 ((struct uart *) 0x40011000)
  #define UART2 ((struct uart *) 0x40004400)



struct gpio {
    volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFRL, AFRH;
  };
  #define GPIOA ((struct gpio *) 0x40020000)
  #define GPIOB ((struct gpio *) 0x40020400)
  #define GPIOC ((struct gpio *) 0x40020800)
  #define GPIOD ((struct gpio *) 0x40020C00)
  #define GPIOE ((struct gpio *) 0x40021000)
  #define GPIOH ((struct gpio *) 0x40021C00)




#endif