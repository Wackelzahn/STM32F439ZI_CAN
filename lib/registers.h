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


// register I2C
struct I2C {
  volatile uint32_t CR1, CR2, OAR1, OAR2, DR, SR1, SR2, CCR, TRISE, FLTR;
};
#define I2C1 ((struct I2C *) 0x40005400)



// register SYSTICK
  struct systick {
    volatile uint32_t CSR, RVR, CVR, CALIB;
  };
  #define SYST ((struct systick *) 0xe000e010)  // 2.2.2

// register NVIC
  struct nvic {
    volatile uint32_t ISER[8];  // Interrupt Set-Enable Register
    uint32_t RESERVED0[24];     // Reserved
    volatile uint32_t ICER[8];  // Interrupt Clear-Enable Register
    uint32_t RESERVED1[24];     // Reserved
    volatile uint32_t  ISPR[8]; // Interrupt Set-Pending Register
    uint32_t  RESERVED2[24];    // Reserved
    volatile uint32_t ICPR[8];  // Interrupt Clear-Pending Register
    uint32_t RESERVED3[24];     // Reserved
    volatile uint32_t IABR[8];  // Interrupt Active-Bit Register
    uint32_t RESERVED4[56];     // Reserved
    volatile uint32_t IPR[240]; // Interrupt Priority Registers (8-bit)
    uint32_t RESERVED5[644];    // Reserved
    volatile uint32_t STIR;     // Software Trigger Interrupt Register
  };
  #define NVIC ((struct nvic *) 0xe000e100) 


// register SCB
  struct scb {
    volatile uint32_t CPUID, ICSR, VTOR, AIRCR, SCR, CCR, SHPR1, SHPR2, SHPR3,
        SHCRS, CFSR, HFSR, DFSR, MMFAR, BFAR, AFSR, PFR0, PFR1, DFR0, AFR0, MMFR0,
        MMFR1, MMFR2, MMFR3, ISAR0, ISAR1, ISAR2, ISAR3, ISAR4, CLIDR, CTR, CCSIDR,
        CSSELR, CPACR, NSACR, RESERVED0[93], STIR, RESERVED1[15], MVFR0, MVFR1, MVFR2;
  };
  #define SCB ((struct scb *) 0xe000ed00)




// CAN register
struct can {
  volatile uint32_t MCR, MSR, TSR, RF0R, RF1R, IER, ESR, BTR, RESERVED1[88], TI0R, 
    TDT0R, TDL0R, TDH0R, TI1R, TDT1R, TDL1R, TDH1R, TI2R, TDT2R, TDL2R, TDH2R,
    RI0R, RDT0R, RDL0R, RDH0R, RI1R, RDT1R, RDL1R, RDH1R, Reserved2[12], FMR,
    FM1R, Reserved3, FS1R, Reserved4, FFA1R, Reserved5, FA1R, Reserved6, 
    Reserved7[7], F0R1, F0R2, F1R1, F1R2, F2R1, F2R2, F3R1, F3R2, F4R1, F4R2, 
    F5R1, F5R2, F6R1, F6R2, F7R1, F7R2, F8R1, F8R2, F9R1, F9R2, F10R1, F10R2, 
    F11R1, F11R2, F12R1, F12R2, F13R1, F13R2, F14R1, F14R2, F15R1, F15R2, F16R1, F16R2,
    F17R1, F17R2, F18R1, F18R2, F19R1, F19R2, F20R1, F20R2, F21R1, F21R2, F22R1, F22R2,
    F23R1, F23R2, F24R1, F24R2, F25R1, F25R2, F26R1, F26R2, F27R1, F27R2;
};
#define CAN1 ((struct can *) 0x40006400)
#define CAN2 ((struct can *) 0x40006800)


// UART1 register
  struct uart {
    volatile uint32_t SR, DR, BRR, CR1, CR2, CR3, GTPR;
  };
  #define UART1 ((struct uart *) 0x40011000)
  #define UART2 ((struct uart *) 0x40004400)


// GPIO register
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