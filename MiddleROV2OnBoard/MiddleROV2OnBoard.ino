/*
 Name:		MiddleROV2Surface.ino
 Created:	06.12.2018 17:37:33
 Author:	Zinkov
*/

#include <Arduino.h>
#include "BrushlessMotor.h"
#include "RotaryCamera.h"

#define RS485_CONTROL_PIN 2

#define FRONT_LEFT_MOTOR_PIN 3
#define FRONT_RIGHT_MOTOR_PIN 4
#define BACK_LEFT_MOTOR_PIN 5
#define BACK_RIGHT_MOTOR_PIN 6
#define VERTICAL_MOTOR_PIN_L 11
#define VERTICAL_MOTOR_PIN_R 12

#define MANIPULATOR_PIN_1 9
#define MANIPULATOR_PIN_2 10
#define CAMERA_PIN 7

BrushlessMotor frontLeftMotor(FRONT_LEFT_MOTOR_PIN);
BrushlessMotor frontRightMotor(FRONT_RIGHT_MOTOR_PIN);
BrushlessMotor backLeftMotor(BACK_LEFT_MOTOR_PIN);
BrushlessMotor backRightMotor(BACK_RIGHT_MOTOR_PIN);
BrushlessMotor verticalMotorL(VERTICAL_MOTOR_PIN_L);
BrushlessMotor verticalMotorR(VERTICAL_MOTOR_PIN_R);

RotaryCamera camera(CAMERA_PIN);

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);
    Serial.println("Setup");
    // Serial1.println("Setup");
    pinMode(RS485_CONTROL_PIN, OUTPUT);
    digitalWrite(RS485_CONTROL_PIN, HIGH);
    frontLeftMotor.init();
    frontRightMotor.init();
    backLeftMotor.init();
    backRightMotor.init();
    verticalMotorL.init();
    verticalMotorR.init();
    camera.init();
    delay(1000);

    digitalWrite(RS485_CONTROL_PIN, LOW);
    Serial.println("Entering loop");
}

void manipulator(int8_t data) {
  if (data > 0) {
    digitalWrite(MANIPULATOR_PIN_1, HIGH);
    digitalWrite(MANIPULATOR_PIN_2, LOW);
  } else if (data < 0) {
    digitalWrite(MANIPULATOR_PIN_1, LOW);
    digitalWrite(MANIPULATOR_PIN_2, HIGH);
  } else {
    digitalWrite(MANIPULATOR_PIN_1, LOW);
    digitalWrite(MANIPULATOR_PIN_2, LOW);
  }
}

#define START_BYTE 0xFE
#define END_BYTE 0xEF

#define DEBUG

void loop() {
    if (Serial1.available()) {
    digitalWrite(LED_BUILTIN, HIGH);
        uint8_t first_received_byte = 0;
        Serial1.readBytes(&first_received_byte, 1);
        if (first_received_byte != START_BYTE){
            Serial.println("ERROR (wrong first byte)");
            return;
        }

        uint8_t buffer[10] = {};
        buffer[0] = first_received_byte;
        Serial1.readBytes(buffer + 1, 9);

        if (buffer[0] == START_BYTE && buffer[9] == END_BYTE) {
            frontLeftMotor.set_power(buffer[1]);
            frontRightMotor.set_power(-buffer[2]-5);
            backLeftMotor.set_power(buffer[3]);
            backRightMotor.set_power(-buffer[4]);
            verticalMotorL.set_power(buffer[5]-4);
            verticalMotorR.set_power(buffer[6]-4);
            camera.rotate(buffer[7] * 3);
            manipulator(buffer[8]);
#ifdef DEBUG
            Serial.print((int8_t)buffer[1]); Serial.print(" ");
            Serial.print((int8_t)buffer[2]); Serial.print(" ");
            Serial.print((int8_t)buffer[3]); Serial.print(" ");
            Serial.print((int8_t)buffer[4]); Serial.print(" ");
            Serial.print((int8_t)buffer[5]); Serial.print(" ");
            Serial.print((int8_t)buffer[6]); Serial.print(" ");
            Serial.print((int8_t)buffer[7]); Serial.print(" ");
            Serial.print((int8_t)buffer[8]); Serial.print(" ");
#endif // DEBUG
        }
        else {
            Serial.println("ERROR");
            Serial.println(Serial1.readString());
        }
    }
}
