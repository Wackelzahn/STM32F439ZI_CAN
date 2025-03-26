#ifndef I2C_H
#define I2C_H

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include "registers.h"


void I2C1_Init(void);
void I2C1_Start (void);
void I2C1_Stop (void);
void I2C1_Write (uint8_t data);
uint8_t I2C1_Read (uint8_t ack);


#endif