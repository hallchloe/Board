////This is a general mapping for a front left ULPDRV board//

#include "DecentralizedLV-Boards.h"
#include "Particle.h"

/////////Swtich this if active on low////////
#define OUTPUT_ON HIGH
#define OUTPUT_OFF LOW


CAN_Controller canController;

DashController_CAN dash(DASH_CONTROL_ADDR); 
PowerController_CAN powerController(POWER_CONTROL_ADDR);

//Driver Pins//

const int PIN_HEADLIGHT_FL = A5;
const int PIN_FAN_FRONT    = D1;
const int PIN_PUMP_FRONT   = S6;
const int PIN_TURN_FL = S4;

//Sense Pins//
const int sensePins[] = {A0, A1, D2, D3, D4, D5, D10};

///////////////////////////////////////////////////////////////
void setup() {

    pinMode(PIN_HEADLIGHT_FL, OUTPUT);
    pinMode(PIN_TURN_FL, OUTPUT);
    pinMode(PIN_FAN_FRONT, OUTPUT);
    pinMode(PIN_PUMP_FRONT, OUTPUT);

    digitalWrite(PIN_HEADLIGHT_FL, LOW);
    digitalWrite(PIN_TURN_FL, LOW);
    digitalWrite(PIN_FAN_FRONT, LOW);
    digitalWrite(PIN_PUMP_FRONT, LOW);

    canController.begin(500000, A2);   // MCP2515 CS pin

}


void loop() {

    LV_CANMessage msg;

    if (canController.receive(msg)) {
        Serial.println("yes ur the baddiest b");
    }

    while (canController.receive(msg)) { //token?
        dash.receiveCANData(msg);
        powerController.receiveCANData(msg);
    }
    //read CAN mg and decode

    //update outputs and states
    bool systemEnabled = powerController.CarOn && !powerController.LowPowerMode;

    bool headlightFL = systemEnabled && dash.headlight;
    bool turnFL = systemEnabled && (dash.leftTurnPWM > 0);
    bool fanOn = systemEnabled && dash.radiatorFan;
    bool pumpOn = systemEnabled && dash.radiatorPump;

    //send status  to GPIO
    digitalWrite(PIN_HEADLIGHT_FL, headlightFL ? OUTPUT_ON : OUTPUT_OFF);
    digitalWrite(PIN_TURN_FL, turnFL ? OUTPUT_ON : OUTPUT_OFF);
    digitalWrite(PIN_FAN_FRONT, fanOn ? OUTPUT_ON : OUTPUT_OFF);
    digitalWrite(PIN_PUMP_FRONT, pumpOn ? OUTPUT_ON : OUTPUT_OFF);

    //Update PMOS 

    //monitor faults
}