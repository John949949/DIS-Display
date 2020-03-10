
#include "mcp_can.h"
extern 	MCP_CAN CAN;
extern 	unsigned long 	DIS_REC_ID;
extern 	unsigned long 	DIS_SEND_ID;
extern	unsigned long	MFSW_REC_ID;
extern	bool	reset;
extern	bool	paused;

unsigned long 	responseTime = 200; 	// 200 msec
unsigned long 	delayStart = 0; 		// the time the delay started
byte 			sendCounter = 0;
byte 			recCounter = 0;		
byte			resetButtCount = 0;
const byte 		LONG_ACK = 1;
const byte 		NORMAL_ACK = 2;
const byte 		NO_ACK = 3;
byte 			expectedAck;
const byte 		KA_MESSAGE[] =         {0xA3};
const byte 		KA_RESPONSE[] =        {0xA1, 0x0F, 0x8A, 0xFF, 0x4A, 0xFF };
const byte 		A0_RESPONSE[] =        {0xA1, 0x0F, 0x8A, 0xFF, 0x4A, 0xFF };
const byte 		RESET_BUTTON[] =       {0x00, 0x00, 0xC0, 0x00, 0x02 };
const byte 		NO_BUTTON[] =   	   {0x00, 0x00, 0x80, 0x00, 0x02 };


void readMFSW(unsigned long id){ 			// check for MFSW button held
	bool messageReceived = false;
	delayStart = millis();
	byte len = 0;
	byte recBuf[8];

	
	while (responseTime >=(millis() - delayStart) && (messageReceived == false)) {
		if((CAN_MSGAVAIL == CAN.checkReceive()) && (messageReceived == false)) {        // check if data coming
			CAN.readMsgBuf(&len, recBuf);    				// read data,  len: data length, buf: data buf
			if (CAN.getCanId() == id) {
				messageReceived = true;
				Serial.print(id, HEX);
				Serial.print("\t");
				for(int i = 0; i<len; i++)  {  			// print the data
					Serial.print(recBuf[i], HEX);
					Serial.print("\t");
				}
				Serial.println();

				if (recBuf[2] == RESET_BUTTON[2]) {			// reset button pressed
					resetButtCount++;
				} else {
					resetButtCount = 0;
				}
				if (resetButtCount > 4)	{					// button held for ? seconds
					reset = true;
					paused = false;
				}
		
			}
		}
	}
}	

	void readDIS(unsigned long id){ 					// read waiting message(s) and send ack
		byte len = 0;
		byte recBuf[8];
		byte ackBuf[8];
		while (CAN_MSGAVAIL == CAN.checkReceive()) {
			CAN.readMsgBuf(&len, recBuf);    			// read data,  len: data length, buf: data buf
			if (CAN.getCanId() == id) {
				Serial.print("status message received");
				Serial.print("\t");
				Serial.print(id, HEX);
				Serial.print("\t");
				for(int i = 0; i<len; i++)  {  			// print the data
					Serial.print(recBuf[i], HEX);
					Serial.print("\t");
				}
				Serial.println();
				if (len > 1) {							// if not an ack or A3
					recCounter++;
					/* Send ack */
					if (recBuf[0] < 0x20) {  					// Don't ack 2X message */
						ackBuf[0] = (0xB0 + recCounter % 15);
						CAN.sendMsgBuf(DIS_SEND_ID, 0, 1, ackBuf);
						Serial.print(DIS_SEND_ID, HEX);
						Serial.print("\t");
						Serial.print(ackBuf[0], HEX);
						Serial.println();
					}
				}
			} else if (CAN.getCanId() == MFSW_REC_ID) {			
				Serial.print("MFSW message received");
				Serial.print("\t");
				Serial.print(MFSW_REC_ID, HEX);
				Serial.print("\t");
				for(int i = 0; i<len; i++)  {  			// print the data
					Serial.print(recBuf[i], HEX);
					Serial.print("\t");
				}
				Serial.println();
				if (recBuf[2] == RESET_BUTTON[2])  {	// reset button pressed 
					paused = true;						// Stop DIS & ECU Comms
				}
			}
		}
	}		



	void sendDIS(unsigned long id, byte len, byte *message){
		byte sendBuf[8]={0};
		bool ack = false;
		for(int i = 0; i < len; i++) {   						// copy message to buffer
			sendBuf[i] = message[i];	
        }		
		if ((sendBuf[0] == 0xA3) || (sendBuf[0] == 0xA0)) {
			expectedAck = LONG_ACK;
		} 
		else {
			sendBuf[0] = (sendBuf[0]) + (sendCounter % 16);		// set sequence count
			sendCounter++;
			if (sendBuf[0] >= 0x20) {
				expectedAck = NO_ACK;
				ack = true;
				delay(100);  									// don't send multi-line messages too fast
			}
			else {
				expectedAck = NORMAL_ACK;
			}
		}
		/* Check for waiting message */
		readDIS(DIS_REC_ID);
		/* Send the message */
		CAN.sendMsgBuf(id, 0, len, sendBuf);
		Serial.print(id, HEX);
		Serial.print("\t");
		for(int i = 0; i < len; i++)    // print the data
			{
			Serial.print(sendBuf[i], HEX);
			Serial.print("\t");
        }
		Serial.println();

	/* Wait for Ack */
		byte ackBuf[8];
		delayStart = millis();
		while (responseTime >=(millis() - delayStart) && (ack == false)) {
			if((CAN_MSGAVAIL == CAN.checkReceive()) && (ack == false))   {        // check if data coming
				CAN.readMsgBuf(&len, ackBuf);    			// read data,  len: data length, buf: data buf
				unsigned long canId = CAN.getCanId();
					if (canId == DIS_REC_ID) {
					Serial.print(canId, HEX);
					Serial.print("\t");
					for(int i = 0; i<len; i++)  {  // print the data
						Serial.print(ackBuf[i], HEX);
						Serial.print("\t");
					}	
					Serial.println();
					switch (expectedAck) {
						case LONG_ACK:
							ack = true;
							for(int i = 0; i<len; i++)  { 
								if (ackBuf[i] != A0_RESPONSE[i]) {
									ack = false;
								}	
							}
						break;
						case NORMAL_ACK :
							if (ackBuf[0] == (0xB0 + (recCounter % 15))); {
								ack = true;
							}	
						break;
					}	
				}
			}
		}
		return ack;
	}
	
	
		
	
	
	void waitDIS(unsigned long id, byte *message){ 			// wait for message
		bool messageReceived = false;
		delayStart = millis();
		byte len = 0;
		byte recBuf[8];
		byte ackBuf[8];

		while (responseTime >=(millis() - delayStart) && (messageReceived == false)) {
			if((CAN_MSGAVAIL == CAN.checkReceive()) && (messageReceived == false)) {        // check if data coming
				CAN.readMsgBuf(&len, recBuf);    				// read data,  len: data length, buf: data buf
				if (CAN.getCanId() == id) {
					if (recBuf[0] == (message[0] + (recCounter % 15))) {
						messageReceived = true;
					}
					Serial.print(id, HEX);
					Serial.print("\t");
					for(int i = 0; i<len; i++)  {  			// print the data
						Serial.print(recBuf[i], HEX);
						Serial.print("\t");
					}
					Serial.println();
					if (messageReceived) {
						recCounter++;
					}
					if (recBuf[0] < 0x20) {  					// Don't ack 2X message */
						ackBuf[0] = (0xB0 + recCounter % 15);
						CAN.sendMsgBuf(DIS_SEND_ID, 0, 1, ackBuf);
						Serial.print(DIS_SEND_ID, HEX);
						Serial.print("\t");
						Serial.print(ackBuf[0], HEX);
						Serial.println();
					}	
				}
			}
		}
	}		
						

	bool disCommsOk(){
		byte len = 0;
		bool ack = false;

		/* check for waiting message */
		readDIS(DIS_REC_ID);
		/* send keepalives */
		CAN.sendMsgBuf(DIS_SEND_ID, 0, sizeof(KA_MESSAGE), KA_MESSAGE);
		Serial.print(DIS_SEND_ID, HEX);
		Serial.print("\t");
		for(int i = 0; i < sizeof(KA_MESSAGE); i++)  {  // print the data
			Serial.print(KA_MESSAGE[i], HEX);
			Serial.print("\t");
		}
		Serial.println();
		
		/* check for keep alive response */
		byte ackBuf[8];
		delayStart = millis();
		while (responseTime >=(millis() - delayStart) && (ack == false)) {
			if((CAN_MSGAVAIL == CAN.checkReceive()) && (ack == false))   {        // check if data coming
				CAN.readMsgBuf(&len, ackBuf);    			// read data,  len: data length, buf: data buf
				unsigned long canId = CAN.getCanId();
				if (canId == DIS_REC_ID) {
					Serial.print(canId, HEX);
					Serial.print("\t");
					for(int i = 0; i<len; i++)  {  // print the data
						Serial.print(ackBuf[i], HEX);
						Serial.print("\t");
					}	
					Serial.println();
					ack = true;
					for(int i = 0; i<len; i++)  { 
						if (ackBuf[i] != KA_RESPONSE[i]) {
							ack = false;
						}	
					}	
				}
			}
		}
		return ack;
	}


		
			
			