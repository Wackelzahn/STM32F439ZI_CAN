#include "INA228.h"


uint32_t temp = 0;

void INA228_Init(void) {
    // Set to bus voltage continuous mode
    //INA228_WriteRegister(0x00, 0x0006);
    I2C1_Init();
}

void INA228_ReadRegister(uint8_t reg, uint8_t *data, uint8_t len) {
    I2C1_Start();                       // Generate start condition
    I2C1_Write(INA228_ADDR << 1);       // Send device address (write)
    I2C1_Write(reg);                    // Send register address
    I2C1_Start();                       // Generate repeated start
    I2C1_Write((INA228_ADDR << 1) | 1); // Send device address (read)
    for (uint8_t i = 0; i < len - 1; i++) {
        data[i] = I2C1_Read(1);      // Read with ACK
    }
    data[len - 1] = I2C1_Read(0);    // Read last byte with NACK
    I2C1_Stop();                     // Generate stop condition
}

void INA228_WriteRegister(uint8_t reg, uint16_t value) {
    I2C1_Start();
    I2C1_Write(INA228_ADDR << 1);       // Write operation to set R/W bit low.
    while (!(I2C1->SR1 & (1U << 1)));   // Wait for ADDR (Slave send Acknowledge bit)
    temp = I2C1->SR1;          // Read first SR1
    temp = I2C1->SR2;                   // then read SR2 to clear ADDR
    I2C1_Write(reg);                    // Register to be accessed
    I2C1_Write((value >> 8) & 0xFFU);    // Write High byte
    I2C1_Write(value & 0xFFU);           // Write Low byte
    I2C1_Stop();
}

float INA228_ReadVBUS(void) {
    uint8_t vbus_data[3];
    INA228_ReadRegister(0x05, vbus_data, 3);
    uint32_t vbus_raw = ((uint32_t)vbus_data[0] << 12) | ((uint32_t)vbus_data[1] << 4) | (vbus_data[2] >> 4);
    if (vbus_raw & 0x80000) vbus_raw |= 0xFFF00000; // Sign extend if negative
    return (float)(vbus_raw * 0.00125);  // Assumes positive voltage; use sign extension if needed

}