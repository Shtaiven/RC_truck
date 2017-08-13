#include <RH_ASK.h>
#include<SPI.h>

RH_ASK driver;  // RC driver

uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
uint8_t buflen = sizeof(buf);

// H-bridge and motor setup
int speedRPin = 3;
int motor1APin = 6;
int motor2APin = 7;

int speedLPin = 5;
int motor3APin = 8;
int motor4APin = 9;

// constants
enum Direction { STOP, FORWARD, BACKWARD };
enum Motor { LEFT, RIGHT };

void transmit() {
    const char *msg = "hello";

    Serial.println("Transmitting...");
    driver.send((uint8_t *)msg, strlen(msg));
    driver.waitPacketSent();
}

void receive() {
//    Serial.println("Receiving...");
    if (driver.recv(buf, &buflen)) // Non-blocking
    { 
      // Message with a good checksum received, dump it.
      Serial.print("Got: ");
      Serial.println((char *)buf);
    }
}

void driveMotor(Motor motor, Direction direction, int speed=0) {
  int motorBridge1 = LOW;
  int motorBridge2 = LOW;
  int motorBridge1Pin, motorBridge2Pin, speedPin;

  if (motor == LEFT) {
    speedPin = speedLPin;
    motorBridge1Pin = motor3APin;
    motorBridge2Pin = motor4APin;
  } else if (motor == RIGHT) {
    speedPin = speedRPin;
    motorBridge1Pin = motor1APin;
    motorBridge2Pin = motor2APin;
  } else {
    return;
  }
  
  if (direction == FORWARD) { motorBridge2 = HIGH; }
  else if (direction == BACKWARD) { motorBridge1 = HIGH; }
  
  // put motor in forward motion
  digitalWrite(motorBridge1Pin, motorBridge1); // set leg 1 of the H-bridge low
  digitalWrite(motorBridge2Pin, motorBridge2); // set leg 2 of the H-bridge high
  // just invert the above values for reverse motion,
  // i.e. motor1APin = HIGH and motor2APin = LOW
  // control the speed 0-255
  analogWrite(speedPin, speed);
}

void setup() {
    Serial.begin(9600);    // Debugging only
    if (!driver.init())
         Serial.println("init failed");

    // set digital i/o pins as outputs for motor and H-bridge: 
    pinMode(speedRPin, OUTPUT);
    pinMode(motor1APin, OUTPUT);
    pinMode(motor2APin, OUTPUT);
    pinMode(speedLPin, OUTPUT);
    pinMode(motor3APin, OUTPUT);
    pinMode(motor4APin, OUTPUT);
}

void loop() {
  receive();
  delay(100);

//  Serial.println("Driving motors forward...");
//  for (int i = 0; i < 10; i++) {
//    Serial.print("Motor speed: ");
//    Serial.println(210 + i*5);
//    driveMotor(LEFT, FORWARD, 210 + i*5);
//    driveMotor(RIGHT, FORWARD, 210 + i*5);
//    delay(1000);
//  }
//
//  Serial.println("Driving motors backward...");
//  for (int i = 0; i < 10; i++) {
//    Serial.print("Motor speed: ");
//    Serial.println(210 + i*5);
//    driveMotor(LEFT, BACKWARD, 210 + i*5);
//    driveMotor(RIGHT, BACKWARD, 210 + i*5);
//    delay(1000);
//  }
//  
//  driveMotor(LEFT, STOP);
//  driveMotor(RIGHT, STOP);
//  delay(200);
}
