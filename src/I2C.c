#include "I2C.h"


// uint32_t temp;

void I2C1_Init(void) {
    // Prepare for PB8 = I1C1_SCL and PB9 = I2C1_SDA

    // enable GPIOB Clock
    RCC->AHB1ENR |= (1 << 1);

    // Configure PB8 for AF4 (I2C1_SCL)
    GPIOB->MODER &= ~(3U << 16);     // Clear MODER[17:16]
    GPIOB->MODER |= (2U << 16);      // Set to alternate function (10b)
    GPIOB->OTYPER |= (1U << 8);      // Set to open-drain
    GPIOB->AFRH &= ~(0xFU << 0);     // Clear AFRH[3:0]
    GPIOB->AFRH |= (4U << 0);        // Set to AF4 (0100b)
    GPIOB->OSPEEDR &= ~(3U << 16);   // medium speed is ok
    GPIOB->OSPEEDR |= (1U << 16);    // Medium speed

    // Configure PB9 for AF4 (I2C1_SDA)
    GPIOB->MODER &= ~(3U << 18);     // Clear MODER[19:18]
    GPIOB->MODER |= (2U << 18);      // Set to alternate function (10b)
    GPIOB->OTYPER |= (1U << 9);      // Set to open-drain
    GPIOB->AFRH &= ~(0xFU << 4);     // Clear AFRH[7:4]
    GPIOB->AFRH |= (4U << 4);        // Set to AF4 (0100b)
    GPIOB->OSPEEDR &= ~(3U << 18);    // medium speed is ok
    GPIOB->OSPEEDR |= (1U << 18);    // Medium speed

    // enable I2C1 clock
    RCC->APB1ENR |= (1U << 21);

    // Reset I2C1
    RCC->APB1RSTR |= (1U << 21);
    RCC->APB1RSTR &= ~(1U << 21);

    // configure I2C1
    I2C1->CR1 = 0;
    I2C1->CR2 = 16; // APB1 clock frequency in MHz
    I2C1->CCR = (16000000 / (2*100000));    // 100kHz
    I2C1->TRISE = 17;   // Maximum rise time

    // enable I2C1
    I2C1->CR1 |= (1U << 0);
   
}

void I2C1_Start (void) {
    I2C1->CR1 |= (1U << 8);  // I2C Start
    while (!(I2C1->SR1 & (1U << 0)));
}


void I2C1_Stop (void) {
    I2C1->CR1 |= (1U << 9); // I2C Stop
}


void I2C1_Write (uint8_t data) {
    I2C1->DR = data;
    
}


uint8_t I2C1_Read (uint8_t ack) {
    if (ack) {
        I2C1->CR1 |= (1U << 10);        // Set ACK bit
    } else {
        I2C1->CR1 &= ~(1U << 10);       // Clear ACK bit (NACK)
    }
    while (!(I2C1->SR1 & (1U << 6)));   // Wait for RxNE (data received)
    return (uint8_t)I2C1->DR;           // Read the byte
}




