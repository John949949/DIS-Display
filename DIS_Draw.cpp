
#include "mcp_can.h"
#include "DIS_Comms.h"

extern	byte 	sendCounter;
extern	byte	recCounter;	

extern void initECU();

const int SPI_CS_PIN = 9;

unsigned long DIS_SEND_ID = 0x6C0;
unsigned long DIS_REC_ID = 0x6C1;


struct canMessage {
  unsigned long ident;
  byte 			messageLength; 
  unsigned char messageData[8];
};


MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

/* Send Messages */
const byte A0_MESSAGE[] =        {0xA0, 0x0F, 0x8A, 0xFF, 0x4A, 0xFF };
const byte M15_SEND[] =          {0x10, 0x15, 0x02, 0x01, 0x02, 0x00, 0x00 };
const byte M01_02_00_SEND[] =    {0x10, 0x01, 0x02, 0x00 };
const byte M01_02_01_SEND[] =    {0x10, 0x01, 0x02, 0x01 };
const byte M01_01_00_SEND[] =    {0x10, 0x01, 0x01, 0x00 };
const byte M08_SEND[] =          {0x10, 0x08};
const byte M24_SEND[] =          {0x10, 0x24};
const byte M28_SEND[] =          {0x10, 0x28};
const byte M09_1_SEND[] =        {0x20, 0x09, 0x20, 0x00, 0x50, 0x07, 0x12,  0x4E };
const byte M09_2_SEND[] =        {0x20, 0x41, 0x56, 0x4C, 0x4C, 0x2D, 0x30,  0x32 };
const byte M09_3_SEND[] =        {0x10, 0x30, 0x30, 0x00 };
const byte M20_SEND[] =          {0x10, 0x20, 0x3B, 0xA0, 0x00 };
const byte M39_SEND[] =          {0x10, 0x39 };
const byte M2F_SEND[] =          {0x10, 0x2F };

const byte R_CLAIM[] =           {0x10, 0x52, 0x05, 0x80, 0x00, 0x1B, 0x40, 0x30 };
const byte R_CLEAR[] =           {0x10, 0x52, 0x05, 0x02, 0x00, 0x1B, 0x40, 0x30 };
const byte DRAW_1[] =            {0x20, 0x52, 0x05, 0x02, 0x00, 0x1B, 0x40, 0x30 };
const byte DRAW_2[] =            {0x20, 0x57, 0x0E, 0x06, 0x00, 0x09, 0x50, 0x57 };
const byte DRAW_3[] =            {0x20, 0x52, 0x65, 0x65, 0x65, 0x65, 0x65, 0x65 };
const byte DRAW_4[] =            {0x20, 0x0B, 0x57, 0x57, 0x0F, 0x06, 0x00, 0x12 };
const byte DRAW_5[] =            {0x20, 0x54, 0x52, 0x51, 0x65, 0x65, 0x65, 0x65 }; 
const byte DRAW_6[] =            {0x20, 0x65, 0x65, 0x1C, 0x4E, 0x0D, 0x57, 0x0F };
const byte DRAW_7[] =            {0x20, 0x06, 0x00, 0x1B, 0x42, 0x0F, 0x0F, 0x73 };
const byte DRAW_8[] =            {0x20, 0x74, 0x65, 0x65, 0x65, 0x65, 0xD1, 0x0D };
const byte DRAW_9[] =            {0x20, 0x42, 0x57, 0x0F, 0x06, 0x00, 0x24, 0x4F };
const byte DRAW_10[] =           {0x20, 0x49, 0x4C, 0x65, 0x65, 0x65, 0x65, 0x65 };
const byte DRAW_11[] =           {0x10, 0x65, 0x1C, 0x6B, 0x43 }; 

byte dataDraw_1[] =          	{0x20, 0x52, 0x05, 0x02, 0x00, 0x1B, 0x40, 0x30 };
byte dataDraw_2[] =          	{0x20, 0x57, 0x07, 0x06, 0x18, 0x09, 0x65, 0x31 };
byte dataDraw_3[] =          	{0x20, 0x35, 0x36, 0x57, 0x07, 0x06, 0x18, 0x12 };
byte dataDraw_4[] =          	{0x20, 0x65, 0x32, 0x35, 0x36, 0x57, 0x07, 0x06 };
byte dataDraw_5[] =          	{0x20, 0x18, 0x1B, 0x31, 0x31, 0x30, 0x30, 0x57 }; 
byte dataDraw_6[] =          	{0x20, 0x07, 0x06, 0x18, 0x24, 0x65, 0x65, 0x35 };
byte dataDraw_7[] =          	{0x10, 0x32 };



const byte A3_MESSAGE[] =        {0xA3};
const byte ACK_B[] =             {0xB0};                                          /*  Also received */
const byte Ack_99[] =            {0x99};

/* Receive Messages */
const byte  M09_1_REC[] =         {0x20, 0x09, 0x20, 0x0B, 0x50, 0x07, 0x29, 0x4A };
const byte  M09_2_REC[] =         {0x10, 0x30, 0x39, 0x00, 0x34, 0x00 };
const byte  M05_1_REC[] =         {0x20, 0x29, 0x09, 0x3B, 0x10, 0xE3, 0x07, 0x4C };
const byte  M05_2_REC[] =         {0x10, 0x13 };

const byte  M00_01_REC[] =        {0x10, 0x00, 0x01 };
const byte  M21_REC[] =           {0x10, 0x21, 0x3B, 0xA0, 0x00 };
const byte  M53_05_REC[] =        {0x10, 0x53, 0x05 };
const byte  M53_84_REC[] =        {0x10, 0x53, 0x84 };
const byte  M53_85_REC[] =        {0x10, 0x53, 0x85 };
const byte  M2E_REC[] =           {0x10, 0x2E };
const byte  M2F_REC[] =           {0x10, 0x2F };
const byte  M39_REC[] =           {0x10, 0x39 };
const byte  M00_02_REC[] =        {0x10, 0x00,  0x02 };

/* Ack Types */
const byte  NORMAL_ACK = 1;
const byte  NINE_ACK = 2;
const byte  A0_ACK = 3;    
const byte  NO_ACK = 4;                                    




void initDIS() {

	sendCounter = 0;
	recCounter = 0;		

    while (CAN_OK != CAN.begin(CAN_100KBPS))              // init can bus : baudrate = 100k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");

    CAN.init_Mask(0, 0, 0x6ff);                         // there are 2 mask in mcp2515, you need to set both of them
    CAN.init_Mask(1, 0, 0x6ff);


    /*
     * set filter, we can receive id from 0x6C0 ~ 0x6C1 & 2C1
     */
    CAN.init_Filt(0, 0, 0x6C0);                          // there are 6 filter in mcp2515
    CAN.init_Filt(1, 0, 0x6C1);                          // there are 6 filter in mcp2515
    CAN.init_Filt(2, 0, 0x2C1);                          // there are 6 filter in mcp2515
//    CAN.init_Filt(3, 0, 0x6C5);                          // there are 6 filter in mcp2515
//    CAN.init_Filt(4, 0, 0x6C4);                          // there are 6 filter in mcp2515
//    CAN.init_Filt(5, 0, 0x6C5);                          // there are 6 filter in mcp2515


 
  /* Establish Comms */

  
    sendDIS(DIS_SEND_ID, sizeof(A0_MESSAGE), A0_MESSAGE);
	delay(10);
    sendDIS(DIS_SEND_ID,  sizeof(M15_SEND), M15_SEND);
    waitDIS(DIS_REC_ID, M00_01_REC);
	delay(10);
    sendDIS(DIS_SEND_ID,  sizeof(M01_02_00_SEND), M01_02_00_SEND);
    waitDIS(DIS_REC_ID, M00_01_REC);
	delay(10);
    for(int i = 0; i<4; i++)  { 
		sendDIS(DIS_SEND_ID,  sizeof(A3_MESSAGE), A3_MESSAGE);
		delay(25);
    }
	delay(10);
    sendDIS(DIS_SEND_ID,  sizeof(M01_02_01_SEND), M01_02_01_SEND);
    waitDIS(DIS_REC_ID, M00_01_REC);
	delay(10);
    sendDIS(DIS_SEND_ID,  sizeof(M01_01_00_SEND), M01_01_00_SEND);
	delay(10);
    sendDIS(DIS_SEND_ID,  sizeof(M08_SEND), M08_SEND);
    waitDIS(DIS_REC_ID, M09_1_REC);
    waitDIS(DIS_REC_ID, M09_2_REC);
	delay(10);
    sendDIS(DIS_SEND_ID,  sizeof(M09_1_SEND), M09_1_SEND);
    sendDIS(DIS_SEND_ID,  sizeof(M09_2_SEND), M09_2_SEND);
    sendDIS(DIS_SEND_ID,  sizeof(M09_3_SEND), M09_3_SEND);
	delay(10);
    sendDIS(DIS_SEND_ID,  sizeof(M24_SEND), M24_SEND);
 	delay(10);
    for(int i = 0; i<4; i++)  { 
		sendDIS(DIS_SEND_ID,  sizeof(A3_MESSAGE), A3_MESSAGE);
		delay(25);
	}
    sendDIS(DIS_SEND_ID,  sizeof(M28_SEND), M28_SEND);
    waitDIS(DIS_REC_ID, M05_1_REC);
    waitDIS(DIS_REC_ID, M05_2_REC);
	delay(10);
    sendDIS(DIS_SEND_ID,  sizeof(M20_SEND), M20_SEND);
    waitDIS(DIS_REC_ID, M21_REC);
}


void claimScreen() {
    sendDIS(DIS_SEND_ID,  sizeof(R_CLAIM), R_CLAIM);
    waitDIS(DIS_REC_ID, M53_84_REC);
    waitDIS(DIS_REC_ID, M53_05_REC);
    waitDIS(DIS_REC_ID, M2E_REC);
	delay(10);
    sendDIS(DIS_SEND_ID,  sizeof(M2F_SEND), M2F_SEND);
	delay(10);
    sendDIS(DIS_SEND_ID,  sizeof(R_CLAIM), R_CLAIM);
    waitDIS(DIS_REC_ID, M53_85_REC);
	delay(10);
    sendDIS(DIS_SEND_ID,  sizeof(R_CLEAR), R_CLEAR);
 	delay(10);
    sendDIS(DIS_SEND_ID,  sizeof(M39_SEND), M39_SEND);
}


void drawFrame() { 

//	sendDIS(DIS_SEND_ID,  sizeof(DRAW_1), DRAW_1);
    sendDIS(DIS_SEND_ID,  sizeof(DRAW_2), DRAW_2);
    sendDIS(DIS_SEND_ID,  sizeof(DRAW_3), DRAW_3);
    sendDIS(DIS_SEND_ID,  sizeof(DRAW_4), DRAW_4);
    sendDIS(DIS_SEND_ID,  sizeof(DRAW_5), DRAW_5);
    sendDIS(DIS_SEND_ID,  sizeof(DRAW_6), DRAW_6);
    sendDIS(DIS_SEND_ID,  sizeof(DRAW_7), DRAW_7);
    sendDIS(DIS_SEND_ID,  sizeof(DRAW_8), DRAW_8);
    sendDIS(DIS_SEND_ID,  sizeof(DRAW_9), DRAW_9);
    sendDIS(DIS_SEND_ID,  sizeof(DRAW_10), DRAW_10);
    sendDIS(DIS_SEND_ID,  sizeof(DRAW_11), DRAW_11);
    sendDIS(DIS_SEND_ID,  sizeof(A3_MESSAGE), A3_MESSAGE);
    sendDIS(DIS_SEND_ID,  sizeof(M39_SEND), M39_SEND);
    sendDIS(DIS_SEND_ID,  sizeof(A3_MESSAGE), A3_MESSAGE);
 
}   

void drawData(int num1, int num2, int num3, int num4) {
	
	char data[4];
	
	sprintf(data, "%4d", num1);
	for(int i = 0; i < 3; i++) {   		// change blanks to "65"
		if (data[i] == 0x20) {
			data[i] = 0x65;
		}
	}
	dataDraw_2[6] = data[0];
	dataDraw_2[7] = data[1];
	dataDraw_3[1] = data[2];
	dataDraw_3[2] = data[3];
	
	sprintf(data, "%4d", num2);
	for(int i = 0; i < 3; i++) {   		// change blanks to "65"
		if (data[i] == 0x20) {
			data[i] = 0x65;
		}
	}
	dataDraw_4[1] = data[0];
	dataDraw_4[2] = data[1];
	dataDraw_4[3] = data[2];
	dataDraw_4[4] = data[3];

	sprintf(data, "%4d", num3);
	for(int i = 0; i < 3; i++) {   		// change blanks to "65"
		if (data[i] == 0x20) {
			data[i] = 0x65;
		}
	}
	dataDraw_5[3] = data[0];
	dataDraw_5[4] = data[1];
	dataDraw_5[5] = data[2];
	dataDraw_5[6] = data[3];

	sprintf(data, "%4d", num4);
	for(int i = 0; i < 3; i++) {   		// change blanks to "65"
		if (data[i] == 0x20) {
			data[i] = 0x65;
		}
	}
	dataDraw_6[5] = data[0];
	dataDraw_6[6] = data[1];
	dataDraw_6[7] = data[2];
	dataDraw_7[1] = data[3];

	
    sendDIS(DIS_SEND_ID,  sizeof(dataDraw_2), dataDraw_2);
    sendDIS(DIS_SEND_ID,  sizeof(dataDraw_3), dataDraw_3);
    sendDIS(DIS_SEND_ID,  sizeof(dataDraw_4), dataDraw_4);
    sendDIS(DIS_SEND_ID,  sizeof(dataDraw_5), dataDraw_5);
    sendDIS(DIS_SEND_ID,  sizeof(dataDraw_6), dataDraw_6);
    sendDIS(DIS_SEND_ID,  sizeof(dataDraw_7), dataDraw_7);
    sendDIS(DIS_SEND_ID,  sizeof(A3_MESSAGE), A3_MESSAGE);
    sendDIS(DIS_SEND_ID,  sizeof(M39_SEND), M39_SEND);
    sendDIS(DIS_SEND_ID,  sizeof(A3_MESSAGE), A3_MESSAGE);
 	
}



