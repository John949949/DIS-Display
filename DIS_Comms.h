#ifndef _DIS_COMMS_H_
#define _DIS_COMMS_H_


    void sendDIS(unsigned long id, byte len, byte *message); 	// send message and wait for ack
	void waitDIS(unsigned long id, byte *message); 				// wait for message and send ack
	void readDIS(unsigned long id);
	bool disCommsOk();
	void readMFSW(unsigned long);

#endif
