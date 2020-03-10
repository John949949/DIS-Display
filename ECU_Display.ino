#include "ECU_Comms.h"
#include "ECU_Read.h"
#include "DIS_Comms.h"
#include "DIS_Draw.h"

unsigned long   MFSW_REC_ID = 0x2C1;

int airFlow, eSpeed, atmPressure, mapPressure, oilTemp, boostP, power, torque = 0;
bool reset = false;
bool paused = false;

void checkComms() {
    if (!ecuCommsOk() || !disCommsOk()) {
        startComms();
    }
}

void startComms() {
   // Start DIS Comms
    initDIS();
    claimScreen();
    drawFrame();
    
   //Start ECU Comms
    initECU();
}

void setup() {
  
    Serial.begin(115200);

    startComms();
}


void loop() {

    if (reset) {
      reset = false;
      paused = false;
      startComms();
    }
    
    if (!paused) {
      checkComms();
    
      airFlow = readMAF();
      eSpeed = readES();
      atmPressure = readATMP();
      mapPressure = readMAP();
      oilTemp = readOT();

      power = calcPower(airFlow);
      torque = calcTorque(airFlow, eSpeed);
      boostP = calcBoost(mapPressure, atmPressure);
   
      Serial.println(airFlow);
      Serial.println(eSpeed);
      Serial.println(atmPressure);
      Serial.println(mapPressure);
      Serial.println(oilTemp);
    
      drawData(power, torque, boostP, oilTemp);
 
      checkComms();
    
    } else {
      readMFSW(MFSW_REC_ID);
    }
    
    delay(200);
  }
