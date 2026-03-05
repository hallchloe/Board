////This is a general mapping for a front left ULPDRV board//

#include "DecentralizedLV-Boards.h"
#include "Particle.h"
#include "Adafruit_GFX_RK.h"

/////////Swtich this if active on low////////
#define OUTPUT_ON HIGH
#define OUTPUT_OFF LOW

void boardConfig();     //Function prototype
//void transmitLPDRVBoards();
void updateOutputPins();
void updateInputPins();
//void updateAnimations();
//void performSpecialFeatures();

CAN_Controller canController;

DashController_CAN dashController(DASH_CONTROL_ADDR); 
PowerController_CAN powerController(POWER_CONTROL_ADDR);
LPDRV_RearLeft_CAN rearLeftDriver(REAR_LEFT_DRIVER);


LV_CANMessage inputMessage; //Input for receiving from multiple addresses and decoding


//Driver Pins//
const int PIN_HEADLIGHT_RL = A5; //Low Power Output 3 (PWM)
const int PIN_FAN_REAR    = D1; //Low Power Output 5 (PWM)
const int PIN_PUMP_REAR   = S6; //Low Power Output 0
const int PIN_TURN_RL = S4; //Low Power Output 1
// const int PIN_GENERAL = S4; //Low Power Output 2


//Sense Pins//
const int sensePins[] = {A0, A1, D2, D3, D4, D5, D10}; //This is according to the schematic on the repo, although the main DecentralizeULPDRV.cpp has D7, D10, A1, A0

#define LP5     D1      //Low Power Output 5 (PWM)
#define LP4     A2      //Low Power Output 4 (PWM)
#define LP3     A5      //Low Power Output 3 (PWM)
#define LP2     S4      //Low Power Output 2
#define LP1     S5      //Low Power Output 1
#define LP0     S6      //Low Power Output 0

#define IP7     A0      //Sense Input 7 (ADC Capable)
#define IP6     A1      //Sense Input 6 (ADC Capable)
#define IP5     D10     //Sense Input 5 
#define IP0     D7      //Sense Input 0

#if USE_SPI == TRUE

// D2 (IP4) is for SPI MOSI when using the Sense Pins for SPI output
// D3 (IP3) is for SPI MISO when using the Sense Pins for SPI output
// D4 (IP2) is for SPI SCK when using the Sense Pins for SPI output
// D5 (IP1) is for SPI Chip-Select when using the Sense Pins for SPI output

#elif USE_UART == TRUE

#define IP4     D2      //Sense Input 4
#define IP3     D3      //Sense Input 3
// D4 (IP2) is for Serial2 TX when using the Sense Pins for UART
// D5 (IP1) is for Serial2 RX when using the Sense Pins for UART

#else

#define IP4     D2      //Sense Input 4
#define IP3     D3      //Sense Input 3
#define IP2     D4      //Sense Input 2
#define IP1     D5      //Sense Input 1

#endif


///Begining of TFT display stuff
#ifdef BDFL
    #define USE_SPI     FALSE
    #define USE_UART    FALSE
#endif

#ifdef BDFR
    #define USE_SPI     TRUE        //Example for hypothetical TFT LCD for displaying AC charge status
    #define USE_UART    FALSE
#endif

#ifdef BDRL
    #define USE_SPI     FALSE
    #define USE_UART    FALSE
#endif

#ifdef BDRR
    #define USE_SPI     FALSE
    #define USE_UART    TRUE        //Example for hypothetical Serial peripheral
#endif

#if USE_SPI == TRUE

// D2 (IP4) is for SPI MOSI when using the Sense Pins for SPI output
// D3 (IP3) is for SPI MISO when using the Sense Pins for SPI output
// D4 (IP2) is for SPI SCK when using the Sense Pins for SPI output
// D5 (IP1) is for SPI Chip-Select when using the Sense Pins for SPI output

#elif USE_UART == TRUE

#define IP4     D2      //Sense Input 4
#define IP3     D3      //Sense Input 3
// D4 (IP2) is for Serial2 TX when using the Sense Pins for UART
// D5 (IP1) is for Serial2 RX when using the Sense Pins for UART

#else

#define IP4     D2      //Sense Input 4
#define IP3     D3      //Sense Input 3
#define IP2     D4      //Sense Input 2
#define IP1     D5      //Sense Input 1

#endif


///////////////////////////////////////////////////////////////
void setup() {

    pinMode(PIN_HEADLIGHT_RL, OUTPUT);
    pinMode(PIN_TURN_RL, OUTPUT);
    pinMode(PIN_FAN_REAR, OUTPUT);
    pinMode(PIN_PUMP_REAR, OUTPUT);

    digitalWrite(PIN_HEADLIGHT_RL, LOW);
    digitalWrite(PIN_TURN_RL, LOW);
    digitalWrite(PIN_FAN_REAR, LOW);
    digitalWrite(PIN_PUMP_REAR, LOW);

    canController.begin(500000, A2);   // MCP2515 CS pin, Low Power Output 4 (PWM)

}


void loop() {
    Serial.begin(9600);
    
    canController.begin(500000, S3);

    canController.addFilter(powerController.boardAddress);
    canController.addFilter(dashController.boardAddress);

    boardConfig();
    updateOutputPins();
    updateInputPins();

    LV_CANMessage msg;

    if (canController.receive(msg)) {
        Serial.println("Who is the best ULVDRV board? I am");
    }

    while (canController.receive(msg)) { //token?
        dashController.receiveCANData(msg);
        powerController.receiveCANData(msg);
        rearLeftDriver.receiveCANData(msg);

    }
    //read CAN msg and decode

    //update outputs and states
    bool systemEnabled = powerController.CarOn && !powerController.LowPowerMode;

    bool headlightFL = systemEnabled && dashController.headlight;
    bool turnFL = systemEnabled && (dashController.leftTurnPWM > 0);
    bool fanOn = systemEnabled && dashController.radiatorFan;
    bool pumpOn = systemEnabled && dashController.radiatorPump;

    //send status  to GPIO
    digitalWrite(PIN_HEADLIGHT_RL, headlightFL ? OUTPUT_ON : OUTPUT_OFF);
    digitalWrite(PIN_TURN_RL, turnFL ? OUTPUT_ON : OUTPUT_OFF);
    digitalWrite(PIN_FAN_REAR, fanOn ? OUTPUT_ON : OUTPUT_OFF);
    digitalWrite(PIN_PUMP_REAR, pumpOn ? OUTPUT_ON : OUTPUT_OFF);

    //Update PMOS 

    //monitor faults
}

void boardConfig(){
    
    //Low Power Outputs
    pinMode(LP5, OUTPUT);
    pinMode(LP4, OUTPUT);
    pinMode(LP3, OUTPUT);
    pinMode(LP2, OUTPUT);
    pinMode(LP1, OUTPUT);
    pinMode(LP0, OUTPUT);

    #ifdef BDFL //Front-Left Driver Board Config
        
        pinMode(IP7, INPUT);
        pinMode(IP6, INPUT);
        pinMode(IP5, INPUT);
        pinMode(IP4, INPUT);
        pinMode(IP3, INPUT);
        pinMode(IP2, INPUT);
        pinMode(IP1, INPUT);
        pinMode(IP0, INPUT);
        
    #endif
    #ifdef BDFR //Front-Right Driver Board Config
       
        pinMode(IP7, INPUT);
        pinMode(IP6, INPUT);
        pinMode(IP5, INPUT);
        pinMode(IP0, INPUT);
        
        //These pins used for LCD control
        //pinMode(IP4, INPUT);
        //pinMode(IP3, INPUT);
        //pinMode(IP2, INPUT);
        //pinMode(IP1, INPUT);

        //Initialize the hypothetical TFT LCD here
        

    #endif
    #ifdef BDRL

        pinMode(IP7, INPUT);
        pinMode(IP6, INPUT);
        pinMode(IP5, INPUT);
        pinMode(IP4, INPUT);
        pinMode(IP3, INPUT);
        pinMode(IP2, INPUT);
        pinMode(IP1, INPUT);
        pinMode(IP0, INPUT);

    #endif
    #ifdef BDRR

        pinMode(IP7, INPUT);
        pinMode(IP6, INPUT);
        pinMode(IP5, INPUT);
        pinMode(IP4, INPUT);
        pinMode(IP3, INPUT);
        //pinMode(IP2, INPUT);      //Not used when using Serial1
        //pinMode(IP1, INPUT);
        pinMode(IP0, INPUT);

        Serial1.begin(115200);      //Begin communication with hypothetical Serial peripheral on the Sense Pins

    #endif
    
}

//Update the state of the pins in this function. Do reads/writes to and from the DecentralizedLV API objects.
void updateOutputPins(){
    #ifdef BDFL //Front-Left Driver Board Config
        
        analogWrite(LP5, dashController.leftTurnPWM);   //Left turn signal PWM. analogWrite creates PWM pulse based on duty cycle (0-255)
        digitalWrite(LP2, dashController.highbeam);     //High-beam output
        digitalWrite(LP1, dashController.headlight);    //Headlight output

    #endif
    #ifdef BDFR //Front-Right Driver Board Config

        analogWrite(LP5, dashController.rightTurnPWM);  //Right turn signal PWM. analogWrite creates PWM pulse based on duty cycle (0-255)
        digitalWrite(LP2, dashController.highbeam);     //High-beam output
        digitalWrite(LP1, dashController.headlight);    //Headlight output

    #endif
    #ifdef BDRL

        if(powerController.BrakeSense) analogWrite(LP4, 255);   //If the brake pedal is pressed, turn on brake lights to full brightness (255)
        else if(dashController.headlight) analogWrite(LP4, 80); //If the brake is not pressed and the headlights are on, then dim the brake lights like a regular car
        else analogWrite(LP4, 0);                               //If brake pedal is not pressed and the headlights are not on, thne the brake lights should be fully off.

        analogWrite(LP3, dashController.leftTurnPWM);           //Left turn signal PWM. analogWrite creates PWM pulse based on duty cycle (0-255)
        
        digitalWrite(LP1, dashController.reversePress);         //If we're in reverse, then turn on the backup lights
        digitalWrite(LP0, dashController.reversePress);         //If we're in reverse, turn on the backup camera power

        rearLeftDriver.bmsFaultInput = digitalRead(IP3);        //Read in the state of the general purpose IO 3. This is the BMS fault switch. Update the flag for this controller
        rearLeftDriver.switchFaultInput = digitalRead(IP1);     //Read in the state of the general purpose IO 1. This is the BMS fault switch. Update the flag for this controller

    #endif
    #ifdef BDRR

        if(powerController.BrakeSense) analogWrite(LP4, 255);   //If the brake pedal is pressed, turn on brake lights to full brightness (255)
        else if(dashController.headlight) analogWrite(LP4, 80); //If the brake is not pressed and the headlights are on, then dim the brake lights like a regular car
        else analogWrite(LP4, 0);                               //If brake pedal is not pressed and the headlights are not on, thne the brake lights should be fully off.

        analogWrite(LP3, dashController.rightTurnPWM);           //Left turn signal PWM. analogWrite creates PWM pulse based on duty cycle (0-255)

        digitalWrite(LP1, dashController.reversePress);         //If we're in reverse, then turn on the backup lights

    #endif
}

/// @brief Read any data this board collects (i.e. switches from the 4X low power inputs) and populate the Boards API variables
void updateInputPins(){
    #ifdef BDFL //Front-Left Driver Board Config
        
        //TO-DO: Read in IP0, IP1, IP2, IP3 here for the functions of the front-right board!
        
    #endif
    #ifdef BDFR //Front-Right Driver Board Config
       
       //TO-DO: Read in IP0, IP1, IP2, IP3 here for the functions of the front-right board!

    #endif
    #ifdef BDRL

        //TO-DO: Read in IP0, IP1, IP2, IP3 here for the functions of the front-right board!
        //An example below of how it was done on SPX
        //rearLeftDriver.bmsFaultInput = digitalRead(IP1);
        //rearLeftDriver.switchFaultInput = digitalRead(IP0);
    
    #endif
    #ifdef BDRR

        //TO-DO: Read in IP0, IP1, IP2, IP3 here for the functions of the front-right board!

    #endif
}