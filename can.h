#ifndef CAN_H
#define CAN_H

#include <stdlib.h>
#include <inttypes.h>
#include "registers.h"

typedef struct CAN_TX_Typedef {     // struct Name
	int id_type;
	int frame_type;
	int send_timestamp;
	int32_t ID;
	int data_length;
	int data[8];
} CAN_TX_Typedef;                    // typedef Name


typedef struct CAN_RX_Typedef {
	int message_timestamp;
	int data_length;
	int data[8];
	int filter_index;
	int frame_type;
	int id_type;
	int32_t ID;
} CAN_RX_Typedef;

typedef struct CAN_Filter_TypeDef {
	int filter_id;
	int enable;
	int id_type;
	int frame_type;
	int type;
	int scale;
	int bank_id;
	uint32_t ID;
} CAN_Filter_TypeDef;

// function declarations
void Can_Init(struct can *can);


#endif