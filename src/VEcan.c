#include "VEcan.h"
#include "can.h"


//variables for VE can for testing
uint16_t chargevoltage = 49100; //max charge voltage in mv
uint16_t chargecurrent = 30000; //max charge current in ma
uint16_t disvoltage = 42000; // max discharge voltage in mv
uint16_t discurrent = 30000; // max discharge current in ma
uint16_t SOH = 100; // SOH place holder

unsigned char mes[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char bmsname[8] = {'C', 'F', 'P', ' ', 'B', 'M', 'S', ' '}; // Comprehensice Full Protection BMS
unsigned char bmsmanu[8] = {'C', '.', 'E', '.', ' ', 'E', 'N', 'G'};
unsigned char snPart1[8] = {'L', 'B', '0', 'A', '0', '1', '0', '0'};
unsigned char snPart2[8] = {'0', '2', '4', '5', '0', '0', '6', '8'};

int SOC =80;

double PackVoltage = 46.7;
float AvgTemperature = 20.5;
uint16_t currentact = 0;

CAN_TX_FRAME threefiftyone;
CAN_TX_FRAME threefiftyfive;
CAN_TX_FRAME threefiftysix;
CAN_TX_FRAME threefiftyA;
CAN_TX_FRAME threefiftyE;
CAN_TX_FRAME threefiftyF;
CAN_TX_FRAME threesixty;
CAN_TX_FRAME threeseventytwo;
CAN_TX_FRAME threeseventythree;
CAN_TX_FRAME threeseventyfour;
CAN_TX_FRAME threeseventyfive;
CAN_TX_FRAME threeseventysix;
CAN_TX_FRAME threeseventyseven;
CAN_TX_FRAME threeseventyeight;     // charged and discharged Energy
CAN_TX_FRAME threeseventynine;      // Installed total capacity
CAN_TX_FRAME threeeighty;           // snPart1
CAN_TX_FRAME threeeightyone;        // snPart2



void VECan_Init () {
    
    // Frame 0x380 Serial No. Part 1
    threeeighty.identifier = 0x0380;
    threeeighty.length = 0x0008U;
    threeeighty.data[0] = 'L';
    threeeighty.data[1] = 'B';
    threeeighty.data[2] = '0';
    threeeighty.data[3] = snPart1[3];
    threeeighty.data[4] = snPart1[4];
    threeeighty.data[5] = snPart1[5];
    threeeighty.data[6] = snPart1[6];
    threeeighty.data[7] = snPart1[7];

    // Frame 0x381 Serial No. Part 2
    threeeightyone.identifier = 0x0381;
    threeeightyone.length = 0x0008U;
    threeeightyone.data[0] = snPart2[0];
    threeeightyone.data[1] = snPart2[1];
    threeeightyone.data[2] = snPart2[2];
    threeeightyone.data[3] = snPart2[3];
    threeeightyone.data[4] = snPart2[4];
    threeeightyone.data[5] = snPart2[5];
    threeeightyone.data[6] = snPart2[6];
    threeeightyone.data[7] = snPart2[7];

    // Frame 0x379 Installed total capacity
    threeseventynine.identifier = 0x0379;
    threeseventynine.length = 0x0002U;
    threeseventynine.data[0] = 0x30;        //  LSB of 0x0230 = 560d
    threeseventynine.data[1] = 0x02;        //  MSB

    // Frame 0x378 charged and discharged energy 
    threeseventyeight.identifier = 0x0378;
    threeseventyeight.length = 0x0008U;
    threeseventyeight.data[0] = 0xA8;   // example 69kw/h -> 0x02A8 = 690
    threeseventyeight.data[1] = 0x02;
    threeseventyeight.data[2] = 0x00;
    threeseventyeight.data[3] = 0x00;
    threeseventyeight.data[4] = 0xB2;
    threeseventyeight.data[5] = 0x02;
    threeseventyeight.data[6] = 0x00;
    threeseventyeight.data[7] = 0x00;



}

void VECan_send () {
    Can_SendMessage(CAN1, &threeeighty);
    Can_SendMessage(CAN1, &threeeightyone);
    Can_SendMessage(CAN1, &threeseventynine);
    Can_SendMessage(CAN1, &threeseventyeight);
}
