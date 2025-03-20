#ifndef CAN_H
#define CAN_H

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include "registers.h"

typedef struct CAN_TX_FRAME 		// struct tag
{    
	uint32_t identifier;
	uint8_t length;
	uint8_t data[8];
} CAN_TX_FRAME; 	// typedef Name


typedef struct CAN_RX_FRAME 
{
	uint32_t identifier;
	uint8_t length;
	uint8_t data[8];
} CAN_RX_FRAME;



// function declarations
void Can_Init (struct can *can);
bool Can_Start (struct can *can);
void Can_Filter (struct can *can, uint16_t identifier);
void Can_SendMessage (struct can *can, CAN_TX_FRAME *TXFrame);
void Can_ReceiveMessage (struct can *can, CAN_RX_FRAME *RXFrame);


#endif