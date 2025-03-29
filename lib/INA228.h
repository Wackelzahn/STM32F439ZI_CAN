#ifndef INA228_H
#define INA228_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "I2C.h"


#define INA228_ADDR 0x40U


bool INA228_ReadRegister(uint8_t reg, uint8_t *data, uint8_t len);
bool INA228_WriteRegister(uint8_t reg, uint16_t value);
bool INA228_ReadVBUS(uint8_t *lsb, uint8_t *msb);
bool INA228_Init(void);
bool INA228_ReadTemp(uint16_t *temp);
bool INA228_ReadCurr(uint32_t *curr);
//bool INA228_ReadShuntV(uint16_t *shunt);
//bool INA228_ReadPower(uint16_t *power);

#endif