#ifndef CAN_H
#define CAN_H

#include <stdlib.h>
#include <inttypes.h>
#include "registers.h"

struct CAN_TX_FRAME 		// struct tag
{    
	uint32_t identifier;
	uint8_t length;
	uint8_t data[8];
} typedef CAN_TX_FRAME; 	// typedef Name


struct CAN_RX_FRAME 
{
	uint32_t identifier;
	uint8_t length;
	uint8_t data[8];
} typedef CAN_RX_FRAME;



// function declarations
void Can_Init (struct can *can);
void Can_Start (struct can *can);
void Can_Filter (struct can *can, uint16_t identifier);
void Can_SendMessage (CAN_TX_FRAME *TXFrame);
void Can_ReceiveMessage (CAN_RX_FRAME *RXFrame);


#endif