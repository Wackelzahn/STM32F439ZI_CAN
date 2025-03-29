#include "INA228.h"


int32_t counter;

bool INA228_Init(void) {
    // Set to bus voltage continuous mode
    
    I2C1_Init();

    // INA228 RESET
    if (INA228_WriteRegister(0x00, 0x8000)){  
    }
    else return false;   // failure

    // INA228 Set ADC sample averaging count to 16
    // Conversion times to 1052 μs
    if (INA228_WriteRegister(0x01, 0xFB6A)){    
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

// Read VBUS voltage
// VBUS = (VSHUNT * 0.00125) * 1310920 / 2^26
// 195.3125 μV/LSB
bool INA228_ReadVBUS(uint16_t *vbus) {
    uint8_t vbus_data[3];
    uint64_t product = 0;
    uint64_t factor = 204800;    // bingo!
    if (!(INA228_ReadRegister(0x05, vbus_data, 3))) return false;  
    uint32_t vbus_raw = ((uint32_t)vbus_data[0] << 12) | ((uint32_t)vbus_data[1] << 4) | (vbus_data[2] >> 4);
    
    product = factor * vbus_raw;   // be carefull. To cast to 64 bit during multiplication, facter needs to be 64 bit
    product = product >> 20;
    *vbus = (uint16_t)(product & 0xFFFF);

    return true;
}

bool INA228_ReadCurr(uint32_t *curr) {
    uint8_t curr_data[3];
    uint32_t factor = 16384;    // bingo!
    uint64_t Current_mA = 0;
    //uint32_t test = 0;
    if (!(INA228_ReadRegister(0x07, curr_data, 3))) return false;  
    uint32_t curr_raw = ((uint32_t)curr_data[0] << 12) | ((uint16_t)curr_data[1] << 4) | ((uint32_t)curr_data[2] >> 4);
    // Sign extend if negative
    if (curr_raw & 0x80000) curr_raw |= 0xFFF00000; 
    
    Current_mA = curr_raw * factor;
    Current_mA = Current_mA >> 14;
    *curr = (uint32_t)(Current_mA & 0xFFFFFFFF);
    
    return true;
}

// Read Shunt voltage
// VSHUNT = (VSHUNT * LSB)  --> 78.125 nV/LSB when ADCRANGE = 1
bool INA228_ReadShuntV(uint32_t *shuntV) {
    uint8_t shuntV_data[3];
    uint32_t factor = 5120;    // bingo!
    uint64_t ShuntV_uV = 0;
    if (!(INA228_ReadRegister(0x04, shuntV_data, 3))) return false;  
    uint32_t shuntV_raw =  ((uint32_t)shuntV_data[0] << 12) | ((uint16_t)shuntV_data[1] << 4) | ((uint32_t)shuntV_data[2] >> 4);
  
    ShuntV_uV = factor * shuntV_raw;
    ShuntV_uV = ShuntV_uV >> 16;
    *shuntV = (uint32_t)(ShuntV_uV);
    
    return true;
}

// Read Power
// Power register holds the value in [mW] for an current lsb of 0.0003125A
// The power value is unsigned!!!
bool INA228_ReadPower(uint32_t *powermW) {
    uint8_t power_data[3];
    if (!(INA228_ReadRegister(0x08, power_data, 3))) return false;  
    uint32_t power_raw = ((uint32_t)power_data[0] << 12) | ((uint16_t)power_data[1] << 4) | ((uint32_t)power_data[2] >> 4);
    *powermW = power_raw;
    
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