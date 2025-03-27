#ifndef INA228_H
#define INA228_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include "I2C.h"


#define INA228_ADDR 0x40U


void INA228_ReadRegister(uint8_t reg, uint8_t *data, uint8_t len);
void INA228_WriteRegister(uint8_t reg, uint16_t value);
uint32_t INA228_ReadVBUS(void);
void INA228_Init(void);


#endif