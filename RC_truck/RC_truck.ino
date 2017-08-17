#include <RH_ASK.h>
#include<SPI.h>

//#define SERIAL_DEBUG

RH_ASK ask_driver;  // RC driver

uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
uint8_t buflen = sizeof(buf);

uint16_t buttons = 0x00;

// H-bridge and motor setup
int SPEED_R_PIN  = 3;
int MOTOR_1A_PIN = 6;
int MOTOR_2A_PIN = 7;
int SPEED_L_PIN  = 5;
int MOTOR_3A_PIN = 8;
int MOTOR_4A_PIN = 9;

// constants
enum Direction {
    MOTOR_STOP,
    MOTOR_FORWARD,
    MOTOR_BACKWARD
};

enum Motor {
    MOTOR_LEFT,
    MOTOR_RIGHT
};

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
    KEY_SELECT    = 0
};

uint16_t receive() {
#ifndef SERIAL_DEBUG
    if (ask_driver.recv(buf, &buflen)) // Non-blocking
    { 
        // Message with a good checksum received, dump it.
        Serial.print("Got: ");
        Serial.println((char *)buf);
    }
#else
    Serial.readBytes(buf, buflen);
#endif  // SERIAL_DEBUG
    return (uint16_t) buf[0] + buf[1] << 8;
}

void driveMotor(Motor motor, Direction direction, int speed=0) {
    int motorBridge1 = LOW;
    int motorBridge2 = LOW;
    int motorBridge1Pin, motorBridge2Pin, speedPin;

    // Set pins based on which motor to control
    if (motor == MOTOR_LEFT) {
        speedPin = SPEED_L_PIN;
        motorBridge1Pin = MOTOR_3A_PIN;
        motorBridge2Pin = MOTOR_4A_PIN;
    } else if (motor == MOTOR_RIGHT) {
        speedPin = SPEED_R_PIN;
        motorBridge1Pin = MOTOR_1A_PIN;
        motorBridge2Pin = MOTOR_2A_PIN;
    } else {
        return;
    }

    // define H-Bridge pins to set motor direction
    if (direction == MOTOR_FORWARD) { motorBridge2 = HIGH; }
    else if (direction == MOTOR_BACKWARD) { motorBridge1 = HIGH; }
  
    // put motor in forward motion
    digitalWrite(motorBridge1Pin, motorBridge1); // set leg 1 of the H-bridge low
    digitalWrite(motorBridge2Pin, motorBridge2); // set leg 2 of the H-bridge high

    // control the speed 0-255
    analogWrite(speedPin, speed);
}

void controlCar(uint16_t controls) {
    if (bitRead(controls, KEY_UP) || bitRead(controls, KEY_TRIANGLE)) {
        Serial.println("Moving forward");
        driveMotor(MOTOR_LEFT, MOTOR_FORWARD, 255);
        driveMotor(MOTOR_RIGHT, MOTOR_FORWARD, 255);
    } else if (bitRead(controls, KEY_DOWN) || bitRead(controls, KEY_X)) {
        Serial.println("Moving backward");
        driveMotor(MOTOR_LEFT, MOTOR_BACKWARD, 255);
        driveMotor(MOTOR_RIGHT, MOTOR_BACKWARD, 255);        
    } else if (bitRead(controls, KEY_LEFT) || bitRead(controls, KEY_SQUARE)) {
        Serial.println("Turning left");
        driveMotor(MOTOR_LEFT, MOTOR_BACKWARD, 255);
        driveMotor(MOTOR_RIGHT, MOTOR_FORWARD, 255);      
    } else if (bitRead(controls, KEY_RIGHT) || bitRead(controls, KEY_CIRCLE)) {
        Serial.println("Turning right");
        driveMotor(MOTOR_LEFT, MOTOR_FORWARD, 255);
        driveMotor(MOTOR_RIGHT, MOTOR_BACKWARD, 255);      
    } else {
        driveMotor(MOTOR_LEFT, MOTOR_STOP);
        driveMotor(MOTOR_RIGHT, MOTOR_STOP);      
    }
}

void setup() {
    Serial.begin(57600);    // Debugging only
    Serial.setTimeout(100);
    
    if (!ask_driver.init())
         Serial.println("init failed");

    // set digital i/o pins as outputs for motor and H-bridge: 
    pinMode(SPEED_R_PIN, OUTPUT);
    pinMode(MOTOR_1A_PIN, OUTPUT);
    pinMode(MOTOR_2A_PIN, OUTPUT);
    pinMode(SPEED_L_PIN, OUTPUT);
    pinMode(MOTOR_3A_PIN, OUTPUT);
    pinMode(MOTOR_4A_PIN, OUTPUT);
}

void loop() {
    buttons = receive();
    controlCar(buttons);
    delay(100);
}
