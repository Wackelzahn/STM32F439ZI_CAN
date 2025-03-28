#include "INA228.h"


int32_t counter;

void INA228_Init(void) {
    // Set to bus voltage continuous mode
    //INA228_WriteRegister(0x00, 0x0006);
    I2C1_Init();
}

bool INA228_ReadRegister(uint8_t reg, uint8_t *data, uint8_t len) {
    I2C1_Start();                       // Generate start condition
    I2C1_Write(INA228_ADDR << 1);       // Send device address (write)
    
    // Wait for address to be acknowledged
    counter = 0;
    while (!(I2C1->SR1 & (1U << 1)))
    {
       if (counter > 1000) {
           return false;
           break;
     }
     counter++;
   };  

    
    // clear the ADDR flag by reading SR1 and then SR2
    uint32_t temp = I2C1->SR1;          // Read first SR1
    temp = I2C1->SR2;                   // then read SR2 to clear ADDR
    (void)temp;                         // Prevent unused variable warning.

    I2C1_Write(reg);                    // Send register address
    
    // Wait for the data register to be empty
  //  counter = 0;
    while (!(I2C1->SR1 & (1U << 7)));
    // {    // TXE bit check
  //  if (counter > 1000) {
   //     return false;
   //     break;
  //  }
  //  counter++;
  //  };  
    
    
    I2C1_Start();                       // Generate repeated start
    I2C1_Write((INA228_ADDR << 1) | 1); // Send device address (read)
    
    // Wait for address to be acknowledged
    counter = 0;
    while (!(I2C1->SR1 & (1U << 1)));
    // {   // Wait for ADDR (Slave send Acknowledge bit)
   // if (counter > 1000) {
    //    return false;
    //    break;
   // }
   // counter++;
   // }; 



    // clear the ADDR flag by reading SR1 and then SR2
    uint32_t temp2 = I2C1->SR1;          // Read first SR1
    temp2 = I2C1->SR2;                   // then read SR2 to clear ADDR
    (void)temp2;                         // Prevent unused variable warning.
    
    for (uint8_t i = 0; i < len - 1; i++) {
        data[i] = I2C1_Read(1);      // Read with ACK
    }
    
    I2C1->CR1 &= ~(1U << 10);        // Clear ACK bit
    I2C1_Stop();                     // Generate stop condition
    data[len - 1] = I2C1_Read(0);    // Read last byte with NACK
    
    return true;


}

void INA228_WriteRegister(uint8_t reg, uint16_t value) {
    I2C1_Start();
    
    // Send the address with the R/W bit set to 0 (write)
    I2C1_Write(INA228_ADDR << 1);       // Write operation to set R/W bit low.
    
    // Wait for address to be acknowledged
    while (!(I2C1->SR1 & (1U << 1)));   // Wait for ADDR (Slave send Acknowledge bit)
    
    // clear the ADDR flag by reading SR1 and then SR2
    uint32_t temp = I2C1->SR1;          // Read first SR1
    temp = I2C1->SR2;                   // then read SR2 to clear ADDR
    (void)temp;                         // Prevent unused variable warning.
    
    // Send the data byte (register to be accessed)
    I2C1_Write(reg);                    // Register to be accessed
    
    // Wait for the data register to be empty
    while (!(I2C1->SR1 & (1U << 7)));    // TXE bit check

    // Send data
    I2C1_Write((value >> 8) & 0xFFU);   // Write High byte
    while (!(I2C1->SR1 & (1U << 7)));    // TXE bit check
    I2C1_Write(value & 0xFFU);          // Write Low byte
    while (!(I2C1->SR1 & (1U << 7)));    // TXE bit check
    I2C1_Stop();
}

bool INA228_ReadVBUS(uint8_t *lsb, uint8_t *msb) {
    uint8_t vbus_data[3];
    uint64_t product = 0;
    uint32_t factor = 1310920;
    if (!(INA228_ReadRegister(0x05, vbus_data, 3))) return false;  
    uint32_t vbus_raw = ((uint32_t)vbus_data[0] << 12) | ((uint32_t)vbus_data[1] << 4) | (vbus_data[2] >> 4);
    if (vbus_raw & 0x80000) vbus_raw |= 0xFFF00000; // Sign extend if negative
    //return (float)(vbus_raw * 0.00125);  // Assumes positive voltage; use sign extension if needed
    
    product = factor * vbus_raw;
    product = product >> 26;
    *lsb = (uint8_t)(product & 0xFF);
    *msb = (uint8_t)((product & 0xFF00) >> 8);

    return true;
}