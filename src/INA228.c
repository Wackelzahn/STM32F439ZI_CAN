#include "INA228.h"


int32_t counter;

bool INA228_Init(void) {
    // Set to bus voltage continuous mode
    
    I2C1_Init();

    // INA228 RESET
    if (INA228_WriteRegister(0x00, 0x8000)){    // INA228 RESET
    }
    else return false;   // failure
    
    // INA228 Set Shunt voltage Range to +/-40.96mV
    // equals max shunt current (250uOhm) at 163.84A
    if (INA228_WriteRegister(0x00, 0x0008)){   
    }                                           
    else return false;   // failure
    
    // INA228 Set Shunt_Cal (0.000250 Ohm)
    // current LSB equals 163.84A / 2'19 = 312.5uA
    // 13107.2 x 10'6 x CURRENT_LSB x RSHUNT x 4 = 4096
    if (INA228_WriteRegister(0x02, 0x1000)){    
    }                                           
    else return false;   // failure   
    
    return true;
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
        }
        counter++;
    };  

    
    // clear the ADDR flag by reading SR1 and then SR2
    uint32_t temp = I2C1->SR1;          // Read first SR1
    temp = I2C1->SR2;                   // then read SR2 to clear ADDR
    (void)temp;                         // Prevent unused variable warning.

    I2C1_Write(reg);                    // Send register address
    
    // Wait for the data register to be empty
    counter = 0;
    while (!(I2C1->SR1 & (1U << 7)))    // TXE bit check
    {                                   
        if (counter > 1000) {
        return false;
        }
        counter++;
    };  
    
    I2C1_Start();                       // Generate repeated start
    I2C1_Write((INA228_ADDR << 1) | 1); // Send device address (read)
    
    // Wait for address to be acknowledged
    counter = 0;
    while (!(I2C1->SR1 & (1U << 1)))
    {  
        if (counter > 1000) {
            return false;
   }
        counter++;
   }; 



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

bool INA228_WriteRegister(uint8_t reg, uint16_t value) 
    {
    I2C1_Start();
    
    // Send the address with the R/W bit set to 0 (write)
    I2C1_Write(INA228_ADDR << 1);       // Write operation to set R/W bit low.
    counter = 0;
    while (!(I2C1->SR1 & (1U << 1)))    // Wait for ADDR (Slave send Acknowledge bit)
    {  
        if (counter > 1000) {
            return false;
        }
        counter++;
    };
    

    // clear the ADDR flag by reading SR1 and then SR2
    uint32_t temp = I2C1->SR1;          // Read first SR1
    temp = I2C1->SR2;                   // then read SR2 to clear ADDR
    (void)temp;                         // Prevent unused variable warning.
    
    // Send the data byte (register to be accessed)
    I2C1_Write(reg);                    // Register to be accessed
    counter = 0;
    while (!(I2C1->SR1 & (1U << 7)))    // TXE bit check
    {  
        if (counter > 1000) {
            return false;
        }
        counter++;
    };

    // Send data
    I2C1_Write((value >> 8) & 0xFFU);   // Write High byte
    counter = 0;
    while (!(I2C1->SR1 & (1U << 7)))   // TXE bit check
    {  
        if (counter > 1000) {
            return false;
        }
        counter++;
    };    

    
    I2C1_Write(value & 0xFFU);          // Write Low byte
    counter = 0;
    while (!(I2C1->SR1 & (1U << 7)))    // TXE bit check
    {  
        if (counter > 1000) {
            return false;
        }
        counter++;
    }; 
    
    
    I2C1_Stop();
   
    return true;
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

bool INA228_ReadCurr(uint32_t *curr) {
    uint8_t curr_data[3];
    uint32_t factor = 16384;    // bingo!
    uint64_t Current_mA = 0;
    // uint32_t curr_raw_test = 0x00000FAF; // 4015 * 312.5 uA = 1254.6875 mA
    // uint64_t curr_raw_test = 0x00005228; // 512000 * 312.5 uA = 160000 mA --->>> 16bit overflow!!!
    if (!(INA228_ReadRegister(0x07, curr_data, 3))) return false;  
    uint32_t curr_raw = ((uint32_t)curr_data[0] << 12) | ((uint16_t)curr_data[1] << 4) | ((uint32_t)curr_data[2] >> 4);
    // (void)curr_raw;
    Current_mA = curr_raw * factor;
    Current_mA = Current_mA >> 14;
    *curr = (uint32_t)(Current_mA);
    
    return true;
}

bool INA228_ReadTemp(uint16_t *temp) {
    uint8_t temp_data[2];
    uint64_t product = 0;
    uint32_t factor = 12802;    // bingo
    if (!(INA228_ReadRegister(0x06, temp_data, 2))) return false;  
    uint16_t temp_raw = ((uint16_t)temp_data[0] << 8) | ((uint16_t)temp_data[1] << 0);
    
    product = factor * temp_raw;
    product = product >> 14;
    *temp = (uint16_t)(product);
    
    return true;
}