#ifndef INA228_H
#define INA228_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "I2C.h"


#define INA228_ADDR 0x40U




bool INA228_ReadRegister(uint8_t reg, uint8_t *data, uint8_t len);
bool INA228_WriteRegister(uint8_t reg, uint16_t value);

// Initialize INA228
// Set Shunt voltage Range to +/-40.96mV
// Set Shunt_Cal (0.000250 Ohm)
bool INA228_Init(void);

// Read VBUS voltage
// VBUS result in mV
bool INA228_ReadVBUS(uint16_t *vbus);


bool INA228_ReadTemp(uint16_t *temp);
bool INA228_ReadCurr(uint32_t *curr);
bool INA228_ReadShuntV(uint32_t *shuntV);
bool INA228_ReadPower(uint32_t *powermW);

#endif