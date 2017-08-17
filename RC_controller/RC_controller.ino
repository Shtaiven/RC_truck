#include <RH_ASK.h>  // Radio transmit/receive library
#include <PS2X_lib.h>  // PS2/X controller protocol library
#include <SPI.h>  // Required import by RH_ASK library

RH_ASK askDriver;  // RC driver
PS2X ps2x; // create PS2 Controller Class

// PSX controller pin assignments
const int CLK_PIN = 9;
const int CMD_PIN = 10;
const int ATT_PIN = 11;
const int DATA_PIN = 13;

// PS2X lib values
byte ps2xType = 0;

// Error values
byte ps2xError = 0;
byte askError = 0;

// PSX button states to be sent over RF
uint8_t buttonState[2] = { 0x00, 0x00 };

void getButtonState() {
    //buttonState[0] = ps2xError + '0';
    //buttonState[1] = askError + '0';
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
    if(!ps2xError && askError) {
        getButtonState();  // place button state in 2 byte buttonState
        transmit(buttonState, 2);  // transmit button states over RF
        delay(200);
    } else {
        digitalWrite(LED_BUILTIN, HIGH);
    }
}
