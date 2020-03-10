
#include "mcp_can.h"
extern 	MCP_CAN ECU_CAN;

extern 	long 	DU_ID3;
extern 	long 	GATE_ID2;

extern const byte 		LONG_ACK = 1;
extern const byte 		NORMAL_ACK = 2;
extern const byte 		NO_ACK = 3;

const byte ECU_KA_MESSAGE[] =    {0xA3};
const byte ECU_KA_RESP[] =       {0xA1, 0x0F, 0x8A, 0xFF, 0x4A, 0xFF };


unsigned long 	ecuResponseTime = 200; 			// 200 msec
unsigned long 	ecuDelayStart = 0; 				// the time the delay started
byte 			ecuSendCounter = 0;
byte 			ecuRecCounter = 0;		
byte 			ecuExpectedAck;


	void sendECU(unsigned long sendId, unsigned long ackId, byte len, byte *message){
		byte sendBuf[8]={0};
		bool ack = false;
		for(int i = 0; i < len; i++) {   // copy message to buffer
			sendBuf[i] = message[i];	
        }		
		if (sendBuf[0] == 0x20) {
			sendBuf[0] = (sendBuf[0]) + (ecuSendCounter % 16);
			ecuSendCounter++;
			ecuExpectedAck = NO_ACK;
			ack = true;
			delay(100);									// Don't send unack'ed messages too fast
		}
		else {
			sendBuf[0] = (sendBuf[0]) + (ecuSendCounter % 16);
			ecuSendCounter++;
			ecuExpectedAck = NORMAL_ACK;
		}
		
		ECU_CAN.sendMsgBuf(sendId, 0, len, sendBuf);
		// print the data
		Serial.print(sendId, HEX);
		Serial.print("\t");
		for(int i = 0; i < len; i++) {    
			Serial.print(sendBuf[i], HEX);
			Serial.print("\t");
        }
		Serial.println();

		/* Wait for ack */

		byte ackBuf[8];
		ecuDelayStart = millis();
		while (ecuResponseTime >=(millis() - ecuDelayStart) && (ack == false)) {
			if((CAN_MSGAVAIL == ECU_CAN.checkReceive()) && (ack == false))   {        // check if data coming
				ECU_CAN.readMsgBuf(&len, ackBuf);    			// read data,  len: data length, buf: data buf
				unsigned long canId = ECU_CAN.getCanId();
				if (canId == ackId) {
					Serial.print(canId, HEX);
					Serial.print("\t");
					for(int i = 0; i<len; i++)  {  // print the data
						Serial.print(ackBuf[i], HEX);
						Serial.print("\t");
					}	
					Serial.println();
					
					if (ackBuf[0] == (0xB0 + (ecuSendCounter % 16))); {
						ack = true;
					}
				}
			}
		}
	}
	
		
	void sendECUNA(unsigned long sendId, byte len, byte *message){
		byte sendBuf[8]={0};
		for(int i = 0; i < len; i++) {   // copy message to buffer
			sendBuf[i] = message[i];	
        }	
		if (sendBuf[0] == 0xA0) {
			ecuSendCounter = 0;			// Reset the send counter if (re)starting comms
			ecuRecCounter = 0;			// Reset the rec counter if (re)starting comms
		}
		ECU_CAN.sendMsgBuf(sendId, 0, len, sendBuf);
		Serial.print(sendId, HEX);
		Serial.print("\t");
		for(int i = 0; i < len; i++)    // print the data
			{
			Serial.print(sendBuf[i], HEX);
			Serial.print("\t");
        }
		Serial.println();	
	}
		
	
	
	void readECU(unsigned long recId, unsigned long ackId, byte *message){ 			// wait for message
		bool messageReceived = false;
		ecuDelayStart = millis();
		byte len = 0;
		byte index = 0;
		byte recBuf[8];
		byte ackBuf[8];

		while ((ecuResponseTime >=(millis() - ecuDelayStart)) && (messageReceived == false)) {
			if(CAN_MSGAVAIL == ECU_CAN.checkReceive()) {      		// check if data coming
				ECU_CAN.readMsgBuf(&len, recBuf);    				// read data,  len: data length, buf: data buf
				if (ECU_CAN.getCanId() == recId) {
					Serial.print(recId, HEX);
					Serial.print("\t");
					for(int i = 0; i<len; i++)  {  					// print the data
						message[index++] = recBuf[i];				// copy receive buffer to message
						Serial.print(recBuf[i], HEX);
						Serial.print("\t");
					}
					Serial.println();
					ecuRecCounter++;
					if (recBuf[0] < 0x20) {  					// If multiple line message don't ack and don't set received
						ackBuf[0] = (0xB0 + ecuRecCounter % 16);
						ECU_CAN.sendMsgBuf(ackId, 0, 1, ackBuf);  	// send Ack
						messageReceived = true;
						Serial.print(ackId, HEX);
						Serial.print("\t");
						Serial.print(ackBuf[0], HEX);
						Serial.println();
					}	
				}
			}
		}
	}
						
	void waitECU(unsigned long recId, byte *message){ 			// check for a particular response - no ack required
		bool ackReceived = false;
		ecuDelayStart = millis();
		byte len = 0;
		byte recBuf[8];

		while (ecuResponseTime >=(millis() - ecuDelayStart) && (ackReceived == false)) {
			if(CAN_MSGAVAIL == ECU_CAN.checkReceive()) {      		// check if data coming
				ECU_CAN.readMsgBuf(&len, recBuf);    				// read data,  len: data length, buf: data buf
				if (ECU_CAN.getCanId() == recId) {
					Serial.print(recId, HEX);
					Serial.print("\t");
					ackReceived = true;
					for(int i = 0; i<len; i++)  {  			
						if (message[i] != recBuf[i]) {
							ackReceived = false;
							Serial.println("ack false");
						}
						Serial.print(recBuf[i], HEX);				// print the data
						Serial.print("\t");
					}
					Serial.println();
				}
			}
		}
		return ackReceived;
	}		
									
	
	bool ecuCommsOk(){
		byte len = 0;
		bool ack = false;

		/* send keepalives */
		ECU_CAN.sendMsgBuf(DU_ID3, 0, sizeof(ECU_KA_MESSAGE), ECU_KA_MESSAGE);
		Serial.print(DU_ID3, HEX);
		Serial.print("\t");
		for(int i = 0; i < sizeof(ECU_KA_MESSAGE); i++) {  // print the data
			Serial.print(ECU_KA_MESSAGE[i], HEX);
			Serial.print("\t");
		}
		Serial.println();
		
		/* check for keep alive response */
		byte ackBuf[8];
		ecuDelayStart = millis();
		while (ecuResponseTime >=(millis() - ecuDelayStart) && (ack == false)) {
			if((CAN_MSGAVAIL == ECU_CAN.checkReceive()) && (ack == false))   {        // check if data coming
				ECU_CAN.readMsgBuf(&len, ackBuf);    			// read data,  len: data length, buf: data buf
				unsigned long canId = ECU_CAN.getCanId();
				if (canId == GATE_ID2) {
					Serial.print(canId, HEX);
					Serial.print("\t");
					for(int i = 0; i<len; i++)  {  // print the data
						Serial.print(ackBuf[i], HEX);
						Serial.print("\t");
					}	
					Serial.println();
					ack = true;
					for(int i = 0; i<len; i++)  { 
						if (ackBuf[i] != ECU_KA_RESP[i]) {
							ack = false;
						}	
					}
				}	
			}
		}
		return ack;
	}
			
			
		
