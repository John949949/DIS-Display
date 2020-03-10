
#include "mcp_can.h"
#include "ECU_Comms.h"

extern byte ecuSendCounter;
extern byte ecuRecCounter;		
extern void initDIS();
extern void drawFrame();

bool 	ecuCommsRunning = false;

const int SPI_CS_PIN = 10;

/* send IDs */
unsigned long DU_ID1 = 0x200;
unsigned long DU_ID2 = 0x32E;
unsigned long DU_ID3 = 0x740;

/* rec IDs */
unsigned long GATE_ID1 = 0x21F;
unsigned long GATE_ID2 = 0x300;
unsigned long GATE_ID3 = 0x201;

MCP_CAN ECU_CAN(SPI_CS_PIN);                                    // Set CS pin

/* Send Messages */
const byte GATE_WAKEUP[] =       {0x1F, 0xC0, 0x00, 0x10, 0x00, 0x03, 0x01 };
const byte GATE_START[] =        {0xA0, 0x0F, 0x8A, 0xFF, 0x32, 0xFF };
const byte GATE_INFO_REQ1[] =    {0x10, 0x00, 0x02, 0x1A, 0x9B };
const byte GATE_INFO_REQ2[] =    {0x10, 0x00, 0x02, 0x1A, 0x91 };
const byte GATE_SIGNOFF[] =    	 {0xA8 };                        // Expected ack = A8

const byte ECU_WAKEUP[] =        {0x01, 0xC0, 0x00, 0x10, 0x00, 0x03, 0x01 };
const byte ECU_START[] =         {0xA0, 0x0F, 0x8A, 0xFF, 0x32, 0xFF };
const byte ECU_DATA1[] =         {0x10, 0x00, 0x02, 0x10, 0x89 };
const byte ECU_DATA2[] =         {0x10, 0x00, 0x02, 0x1A, 0x9B };
const byte ECU_DATA3[] =         {0x10, 0x00, 0x04, 0x31, 0xB8, 0x00, 0x00 };
const byte ECU_DATA4[] =         {0x10, 0x00, 0x02, 0x1A, 0x9A };
const byte ECU_DATA5[] =         {0x10, 0x00, 0x02, 0x1A, 0x91 };
const byte ECU_BLOCK1[] =        {0x10, 0x00, 0x02, 0x21, 0x01 };
const byte ECU_BLOCK2[] =        {0x10, 0x00, 0x02, 0x21, 0x02 };
const byte ECU_BLOCK115[] =      {0x10, 0x00, 0x02, 0x21, 0x73 };
const byte ECU_BLOCK113[] =      {0x10, 0x00, 0x02, 0x21, 0x71 };
const byte ECU_BLOCK7[] =     	 {0x10, 0x00, 0x02, 0x21, 0x07 };		// Blocks over 127 are upper values of block (X - 127)

const byte A3_MESSAGE[] =        {0xA3};
const byte ACK_B[] =             {0xB0};                                          /*  Also received */
const byte Ack_99[] =            {0x99};

/* Receive Messages */
const byte  GATE_WAKE_RESP[] =    {0x00, 0xD0, 0x00, 0x03, 0x2E, 0x03, 0x01 };
const byte  GATE_START_RESP[] =   {0xA1, 0x0F, 0x8A, 0xFF, 0x4A, 0xFF };

const byte  ECU_WAKE_RESP[] =     {0x00, 0xD0, 0x00, 0x03, 0x40, 0x07, 0x01 };
const byte  ECU_START_RESP[] =    {0xA1, 0x0F, 0x8A, 0xFF, 0x4A, 0xFF };
const byte  ECU_DATA1_RESP[] =    {0x10, 0x00, 0x02, 0x50, 0x89, };
const byte  ECU_A3_RESP[] =       {0xA1, 0x0F, 0x8A, 0xFF, 0x4A, 0xFF };

/* Ack Types */
const byte  NORMAL_ACK = 1;
const byte  NINE_ACK = 2;
const byte  A0_ACK = 3;    
const byte  NO_ACK = 4;                                    


void initECU() {

	ecuSendCounter = 0;
	ecuRecCounter = 0;
	byte buffer[64] = {0};										// big enough for multi-line messages
  
    while (CAN_OK != ECU_CAN.begin(CAN_500KBPS))              	// init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    Serial.println("CAN BUS Shield init ok!");					// no need to mask as no other comms on this bus

// 		ECU_CAN.init_Mask(0, 0, 0x6FF);                         // there are 2 mask in mcp2515, you need to set both of them
// 		ECU_CAN.init_Mask(1, 0, 0x6FF);


    /*
     * set filter, we can receive id from 0x2C0 ~ 0x2C5
     */
// 		ECU_CAN.init_Filt(0, 0, 0x6C0);                          // there are 6 filter in mcp2515
// 		ECU_CAN.init_Filt(1, 0, 0x6C1);                          // there are 6 filter in mcp2515
//    	ECU_CAN.init_Filt(2, 0, 0x6C4);                          // there are 6 filter in mcp2515
//    	ECU_CAN.init_Filt(3, 0, 0x6C5);                          // there are 6 filter in mcp2515
//    	ECU_CAN.init_Filt(4, 0, 0x6C4);                          // there are 6 filter in mcp2515
//    	ECU_CAN.init_Filt(5, 0, 0x6C5);                          // there are 6 filter in mcp2515

 
	/* Establish Comms */
	
	/* Gateway Comms */
    /*  Not required if ECU address is known */
	
/*  sendECUNA(DU_ID1, sizeof(GATE_WAKEUP), GATE_WAKEUP);
	waitECU(GATE_ID1, GATE_WAKE_RESP);
    sendECUNA(DU_ID2, sizeof(GATE_START), GATE_START);
    waitECU(GATE_ID2, GATE_START_RESP);
    sendECU(DU_ID2, GATE_ID2, sizeof(GATE_INFO_REQ1), GATE_INFO_REQ1);
	readECU(GATE_ID2, DU_ID2, buffer);
	sendECU(DU_ID2, GATE_ID2, sizeof(GATE_INFO_REQ2), GATE_INFO_REQ2);
    readECU(GATE_ID2, DU_ID2, buffer);
 	sendECUNA(DU_ID2, sizeof(GATE_SIGNOFF), GATE_SIGNOFF);
    waitECU(GATE_ID2, GATE_SIGNOFF);
*/
	
	/* Start ECU Comms */

    sendECUNA(DU_ID1, sizeof(ECU_WAKEUP), ECU_WAKEUP);
    waitECU(GATE_ID3, ECU_WAKE_RESP);
    sendECUNA(DU_ID3, sizeof(ECU_START), ECU_START);
    waitECU(GATE_ID2, ECU_START_RESP);
    sendECU(DU_ID3, GATE_ID2, sizeof(ECU_DATA1), ECU_DATA1);
    readECU(GATE_ID2, DU_ID3,  buffer);
    sendECU(DU_ID3, GATE_ID2, sizeof(ECU_DATA2), ECU_DATA2);
    readECU(GATE_ID2, DU_ID3,  buffer);
    sendECU(DU_ID3, GATE_ID2, sizeof(ECU_DATA3), ECU_DATA3);
    readECU(GATE_ID2, DU_ID3,  buffer);
    sendECU(DU_ID3, GATE_ID2, sizeof(ECU_DATA4), ECU_DATA4);
    readECU(GATE_ID2, DU_ID3,  buffer);
    sendECUNA(DU_ID3, sizeof(A3_MESSAGE), A3_MESSAGE);
    waitECU(GATE_ID2, ECU_A3_RESP);
    sendECU(DU_ID3, GATE_ID2, sizeof(ECU_DATA5), ECU_DATA5);
    readECU(GATE_ID2, DU_ID3,  buffer);
    sendECUNA(DU_ID3, sizeof(A3_MESSAGE), A3_MESSAGE);
    waitECU(GATE_ID2, ECU_A3_RESP);
	ecuCommsRunning = true;
}



int readMAF() {		
	byte buffer[32] = {0};
	bool prompt;
	float a, b;
	int result;
	sendECU(DU_ID3, GATE_ID2, sizeof(ECU_BLOCK2), ECU_BLOCK2);
    readECU(GATE_ID2, DU_ID3, buffer);
	
// buffer contains 4 x 8 bytes and 8 data values 
// Block x values 1A = [6], 1B = [7], 2A = [10], 2B = [11], 3A = [13], 3B = [14], 4A = [17], 4B = [18]
// Block x + 128 values 1A = [20], 1B = [21], 2A = [23], 2B = [25], 3A = [27], 3B = [28], 4A = [30], 4B = [31]
// AirFlow is block 2 data 4
// Airflow = (B * 1.421) + (A / 182) g/S  
	a = float(buffer[17]);
	b = float(buffer[18]);
	result  = int((float(buffer[18]) * 1.421) + (float(buffer[17]) / 182));
//	result = buffer[18];
	return result;
}


int readES() {			
	byte buffer[32] = {0};
	bool prompt;
	int a, b, result;
	sendECU(DU_ID3, GATE_ID2, sizeof(ECU_BLOCK1), ECU_BLOCK1);
    readECU(GATE_ID2, DU_ID3, buffer);
	
// buffer contains 4 x 8 bytes and 8 data values 
// Block x values 1A = [6], 1B = [7], 2A = [10], 2B = [11], 3A = [13], 3B = [14], 4A = [17], 4B = [18]
// Block x + 128 values 1A = [20], 1B = [21], 2A = [23], 2B = [25], 3A = [27], 3B = [28], 4A = [30], 4B = [31]
// E speed is block 1 data 1
// E speed = A * B / 5 RPM
	a = int(buffer[6]);
	b = int(buffer[7]);
	if ((a * b) < 5 ){
		result = 0;
	} else {
		result  = (a * b) / 5;
	}
	return result;

}

int readMAP() {
	byte buffer[32] = {0};
	bool prompt;
	int a, b, result;
	sendECU(DU_ID3, GATE_ID2, sizeof(ECU_BLOCK115), ECU_BLOCK115);
    readECU(GATE_ID2, DU_ID3, buffer);
	
// buffer contains 4 x 8 bytes and 8 data values 
// Block x values 1A = [6], 1B = [7], 2A = [10], 2B = [11], 3A = [13], 3B = [14], 4A = [17], 4B = [18]
// Block x + 128 values 1A = [20], 1B = [21], 2A = [23], 2B = [25], 3A = [27], 3B = [28], 4A = [30], 4B = [31]
// MAP is block 115 data 4
// MAP = A * B / 25 mBar
	a = int(buffer[17]);
	b = int(buffer[18]);
	if ((a * b) < 10 ){
		result = 0;
	} else {
		result  = (a * b) / 10;
	}
	return result;
}

int readATMP() {
	byte buffer[32] = {0};
	bool prompt;
	int a, b, result;
	sendECU(DU_ID3, GATE_ID2, sizeof(ECU_BLOCK113), ECU_BLOCK113);
    readECU(GATE_ID2, DU_ID3, buffer);
	
// buffer contains 4 x 8 bytes and 8 data values 
// Block x values 1A = [6], 1B = [7], 2A = [10], 2B = [11], 3A = [13], 3B = [14], 4A = [17], 4B = [18]
// Block x + 128 values 1A = [20], 1B = [21], 2A = [23], 2B = [25], 3A = [27], 3B = [28], 4A = [30], 4B = [31]
// ATMP is block 113 data 4
// ATMP = A * B / 25 mBar
	a = int(buffer[17]);
	b = int(buffer[18]);
	if ((a * b) < 25 ){
		result = 0;
	} else {
		result  = (a * b) / 25;
	}
	return result;
}

int readOT() {			
	byte buffer[32] = {0};
	bool prompt;
	int a, b, result;
	sendECU(DU_ID3, GATE_ID2, sizeof(ECU_BLOCK7), ECU_BLOCK7);
    readECU(GATE_ID2, DU_ID3, buffer);
	
// buffer contains 4 x 8 bytes and 8 data values 
// Block x values 1A = [6], 1B = [7], 2A = [10], 2B = [11], 3A = [13], 3B = [14], 4A = [17], 4B = [18]
// Block x + 128 values 1A = [20], 1B = [21], 2A = [23], 2B = [25], 3A = [27], 3B = [28], 4A = [30], 4B = [31]
// Oil Temp is block 134 data 1
// 1A Temp = B - A
	a = int(buffer[21]);
	b = int(buffer[20]);
	result  =  a - b;
	return result;
}


int calcPower(int maf){
	float temp;
	temp = float(maf) / 0.932;
	return(int(temp));
}


int calcTorque(int maf, int es){
	float temp;
	temp = ((float(maf) * 10250 )/ float(es) );
	return(int(temp));
}


int calcBoost(int map, int atmp){
	int boost = map - atmp;
	if (boost < 0) {
		boost = 0;
	}
	return boost;
}

