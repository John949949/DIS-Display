#ifndef _ECU_COMMS_H_
#define _ECU_COMMS_H_

	void sendECUNA(unsigned long recId, byte len, byte *message); 						// send message don't wait for ack
    void sendECU(unsigned long sendId, unsigned long ackId, byte len, byte *message); 	// send message and wait for ack
	void waitECU(unsigned long recId, byte *message); 									// wait for message and send ack
	void readECU(unsigned long recId, unsigned long ackId, byte *message); 				// read message and send ack
	bool ecuCommsOk();
	
#endif
