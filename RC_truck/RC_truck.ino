#include <RH_ASK.h>  // This library must be configured to use TIMER2
#include <SPI.h>
#include <Servo.h>

/****************************** Global Constants ******************************/

//#define SERIAL_DEBUG
#define RUN_DEMO

// Servo constants
const int SERVO_R_PIN = 4;
const int SERVO_L_PIN = 5;
const int SERVO_C_PIN = 7;

enum Direction {
    STOP,
    FORWARD,
    BACKWARD,
    CLOCKWISE,
    COUNTERCLOCKWISE
};

// RH_ASK constants
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

/****************************** Global Variables ******************************/

RH_ASK ask_driver;  // RC driver

Servo servo_left;
Servo servo_right;
Servo servo_center;

uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
uint8_t buflen = sizeof(buf);

uint16_t buttons = 0x00;

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

/****************************** Functions ******************************/

void driveWheels(Direction direction) {
    switch (direction) {
        case STOP:
            servo_left.write(90);
            servo_right.write(90);
            break;
        case FORWARD:
            servo_left.write(0);
            servo_right.write(180);
            break;
        case BACKWARD:
            servo_left.write(180);
            servo_right.write(0);
            break;
        case CLOCKWISE:
            servo_left.write(0);
            servo_right.write(0);
            break;
        case COUNTERCLOCKWISE:
            servo_left.write(180);
            servo_right.write(180);
            break;
        default:
            break;
    }
}

void runDemo(void) {
    Serial.println("Forward");
    driveWheels(FORWARD);
    delay(1000);
    Serial.println("Backward");
    driveWheels(BACKWARD);
    delay(1000);
    Serial.println("CW");
    driveWheels(CLOCKWISE);
    delay(1000);
    Serial.println("CCW");
    driveWheels(COUNTERCLOCKWISE);
    delay(1000);
    Serial.println("Stop");
    driveWheels(STOP);
    delay(1000);
}

void controlCar(uint16_t controls) {
    if (bitRead(controls, KEY_UP) || bitRead(controls, KEY_TRIANGLE)) {
        Serial.println("Moving forward");
        driveWheels(FORWARD);
    } else if (bitRead(controls, KEY_DOWN) || bitRead(controls, KEY_X)) {
        Serial.println("Moving backward");
        driveWheels(BACKWARD);       
    } else if (bitRead(controls, KEY_LEFT) || bitRead(controls, KEY_SQUARE)) {
        Serial.println("Turning left");
        driveWheels(COUNTERCLOCKWISE);     
    } else if (bitRead(controls, KEY_RIGHT) || bitRead(controls, KEY_CIRCLE)) {
        Serial.println("Turning right");
        driveWheels(CLOCKWISE);    
    } else {
        driveWheels(STOP);  
    }
}

/****************************** Program Entry ******************************/

void setup() {
    Serial.begin(57600);    // Debugging only
    Serial.setTimeout(100);
    
    if (!ask_driver.init())
         Serial.println("init failed");

    // Attach servo pins
    servo_left.attach(SERVO_L_PIN);
    servo_right.attach(SERVO_R_PIN);
    servo_center.attach(SERVO_C_PIN);
    driveWheels(STOP);
}

void loop() {
    #ifndef RUN_DEMO
        buttons = receive();
        controlCar(buttons);
        delay(100);
    #else
        runDemo();
    #endif // RUN_DEMO
}
