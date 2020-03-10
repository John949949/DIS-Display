#ifndef _ECU_READ_H_
#define _ECU_READ_H_

void initECU();

int readMAF();
int readES();
int readMAP();
int readATMP();
int readOT();

int calcPower(int maf);
int calcTorque(int maf, int es);
int calcBoost(int atmPress, int mapPress);

#endif