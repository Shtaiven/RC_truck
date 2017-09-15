#include <RH_ASK.h>  // Radio transmit/receive library
#include <PS2X_lib.h>  // PS2/X controller protocol library
#include <SPI.h>  // Required import by RH_ASK library

/****************************** Global Constants ******************************/

#define RUN_DEMO  // Comment this out to send controller button signals

// PSX controller pin assignments
const int CLK_PIN = 9;
const int CMD_PIN = 10;
const int ATT_PIN = 11;
const int DATA_PIN = 13;

// Playstations controller bit positions
enum Key {
    KEY_UP        = 4,
    KEY_DOWN      = 6,
    KEY_LEFT      = 7,
    KEY_RIGHT     = 5,
    KEY_X         = 14,
    KEY_SQUARE    = 15,
    KEY_TRIANGLE  = 12,
    KEY_CIRCLE    = 13,
    KEY_L1        = 10,
    KEY_L2        = 8,
    KEY_R1        = 11,
    KEY_R2        = 9,
    KEY_START     = 3,
    KEY_SELECT    = 0,
    KEY_NONE      = 16
};

/****************************** Global Variables ******************************/

RH_ASK askDriver;  // RC driver
PS2X ps2x; // create PS2 Controller Class

// PS2X lib values
byte ps2xType = 0;

// Error values
byte ps2xError = 0;
byte askError = 0;

// PSX button states to be sent over RF
uint8_t buttonState[2] = { 0x00, 0x00 };

/****************************** Functions ******************************/

void getButtonState() {
    uint16_t buttons = ps2x.ButtonDataByte();
    buttonState[0] = (uint8_t) buttons;
    buttonState[1] = (uint8_t) (buttons >> 8);
    Serial.println(buttons);
}

void transmit(uint8_t *data, int lenData) {
    digitalWrite(LED_BUILTIN, HIGH);
    askDriver.send(data, lenData);
    askDriver.waitPacketSent();
    digitalWrite(LED_BUILTIN, LOW);
}

void setButtonState(Key key) {
    buttonState[0] = 0;
    buttonState[1] = 0;
    if (key > 7) {
        bitSet(buttonState[0], key - 8);
    } else {
        bitSet(buttonState[1], key);
    }
}

void sendKey(Key key, float seconds = 0.1) {
    setButtonState(key);
    seconds *= 5;
    int tempSec = (int) seconds;
    for (int i = 0; i < tempSec; ++i) {
        transmit(buttonState, 2);  // transmit button states over RF
        delay(100);
    }
}

void runDemo() {
    float turn90Deg = 1.0;
    if(askError) {
        sendKey(KEY_L2   , 2.5);
        sendKey(KEY_UP   , 3.5);
        sendKey(KEY_RIGHT, turn90Deg);
        sendKey(KEY_UP   , 1.0);
        sendKey(KEY_LEFT , turn90Deg);
        sendKey(KEY_UP   , 3.5);
        sendKey(KEY_LEFT , turn90Deg);
        sendKey(KEY_UP   , 3.5);
        sendKey(KEY_LEFT , turn90Deg);
        sendKey(KEY_NONE , 2.0);
        sendKey(KEY_DOWN , turn90Deg);
        sendKey(KEY_NONE , 1.0);
        sendKey(KEY_L1   , 2.5);
        sendKey(KEY_L2   , 2.5);
    }
}

/****************************** Program Entry ******************************/

void setup() {
    Serial.begin(57600);  // Debugging only

    // Driver and pin setup
    pinMode(LED_BUILTIN, OUTPUT);  // Debug LED
    askError = askDriver.init();
    if(!askError) {  // setup RH ASK driver
        Serial.println("RH ASK init failed");
    }
    ps2xError = ps2x.config_gamepad(CLK_PIN, CMD_PIN, ATT_PIN, DATA_PIN);  //setup PSX pins
    if(ps2xError) {
        Serial.println("PS2X init failed");
    }
    ps2xType = ps2x.readType();
    switch(ps2xType) {
        case 0:
            Serial.println("Unknown controller type");
            break;
        case 1:
            Serial.println("DualShock controller found");
            break;
        case 2:
            Serial.println("GuitarHero controller found");
            break;
    }
}

void loop() {
    #ifndef RUN_DEMO
        if(!ps2xError && askError) {
            getButtonState();  // place button state in 2 byte buttonState
            transmit(buttonState, 2);  // transmit button states over RF
            delay(100);
        } else {
            digitalWrite(LED_BUILTIN, HIGH);
        }
     #else
        runDemo();
     #endif
}
