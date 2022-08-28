#include "GamepadUtils.h"

#define SERIAL_DEBUG    Serial          // USB Serial port (to PC)
//#define SERIAL_CONTROL  Serial1         // Arduino Leonardo
#define SERIAL_CONTROL  Serial3      // Arudino MEGA

//#define DEBUG_OUTPUT

#define RS485_CONTROL_PIN 2
#define START_BYTE 0xFE
#define END_BYTE 0xEF

constexpr int MAX_POWER = 100;

int treshold(int v, int b) {
    if (abs(v) < b) return 0;
    return v;
}

int filterAxis(byte axis) {
    int an = getStickState(axis);
    an -= 127;
    an = treshold(an, 10) * -1;
    an = constrain(an, -127, 127);
    int val = map(an, -127, 127, -MAX_POWER, MAX_POWER);
    return val;
}
/*
int8_t* getLagPower(int x) {
    static int8_t MyArray[4]={constrain(-x, -MAX_POWER, MAX_POWER),constrain(x, -MAX_POWER, MAX_POWER),constrain(x, -MAX_POWER, MAX_POWER),constrain(-x, -MAX_POWER, MAX_POWER)};
      return MyArray;
}*/

/*int8_t getLeftPower(int y, int x) {
    return constrain(x + y, -MAX_POWER, MAX_POWER);
}

int8_t getRightPower(int y, int x) {
    return constrain(x - y, -MAX_POWER, MAX_POWER);
}
*/
int8_t getFrontLeftPower(int y, int x) {
    if(x>0){
      return constrain(10, -MAX_POWER, MAX_POWER) / getSpeedDivider();
      
      }else{
        if(x < 0){
      return constrain(0, -MAX_POWER, MAX_POWER) / getSpeedDivider();
        }
        }

    if(y > 0){
      return constrain(10, -MAX_POWER, MAX_POWER) / getSpeedDivider();
      
      }else{
        if(y < 0){
      return constrain(-10, -MAX_POWER, MAX_POWER) / getSpeedDivider();
        }
        }


}

int8_t getFrontRightPower(int y, int x) {
    return constrain(y - x, -MAX_POWER, MAX_POWER);
}

int8_t getBackLeftPower(int y, int x) {
    return constrain(y + x, -MAX_POWER, MAX_POWER);
}

int8_t getBackRightPower(int y, int x) {
    return constrain(y - x, -MAX_POWER, MAX_POWER);
}


int8_t getVerticalPower(int z) {
    return -constrain(z, -MAX_POWER, MAX_POWER);
}


int8_t checkBtn(uint16_t btn1, uint16_t btn2) {
    if (getButtonState(btn1)) {
        return 1;
    }
    if (getButtonState(btn2)) {
        return -1;
    }
    return 0;
}

int8_t getCamera() {
    int speed = 1;
    return checkBtn(buttonR1, buttonR2) * -speed;
}

int8_t getManipulator() {
    int speed = 1;
    return checkBtn(buttonL1, buttonL2) * speed;
}

int8_t getSpeedDivider() {
    int multiplier = checkBtn(buttonSquare, buttonCircle);
    if (multiplier == 0) {
        return 2;
    } else if (multiplier == 1) {
        return 4;
    } else if (multiplier == -1) {
        return 1;
    }
}

void setup() {
    SERIAL_CONTROL.begin(115200);  // control serial port (to ROV)
    SERIAL_DEBUG.begin(115200);   // debug serial port (to PC)

    pinMode(RS485_CONTROL_PIN, OUTPUT);
    digitalWrite(RS485_CONTROL_PIN, HIGH);

    SERIAL_DEBUG.println(F("Starting MiddleROV…"));
}

int8_t getMotorButton() {
	int speed = 1;
	return checkBtn(buttonTriangle, buttonCross) * speed;
}

void loop() {
    readGamepad();

    int linear_x, linear_y, linear_z, angular_z = 0;
    linear_z = filterAxis(stickLY);
    linear_y = filterAxis(stickLX);
    linear_x = filterAxis(stickRY);
    angular_z = filterAxis(stickRX);

#ifdef DEBUG_OUTPUT
    printButtons();

    SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print(getStickState(stickLY)); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print(getStickState(stickLX)); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print(getStickState(stickRY)); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.println("");

    SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print(y); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print(x); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print(z); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.println("");
#endif

    uint8_t buffer[10];
    buffer[0] = START_BYTE;
    if(linear_x>0){
      buffer[1] = constrain(-linear_x, -MAX_POWER, MAX_POWER);
      buffer[2] = constrain(linear_x, -MAX_POWER, MAX_POWER);
      buffer[3] = constrain(-linear_x,-MAX_POWER, MAX_POWER)  ;
      buffer[4] = constrain(-linear_x, -MAX_POWER, MAX_POWER);
      SERIAL_DEBUG.print("t");
      }else{
        
      buffer[1] = constrain(-linear_x, -MAX_POWER, MAX_POWER)/ getSpeedDivider();
      buffer[2] = constrain(linear_x, -MAX_POWER, MAX_POWER)/ getSpeedDivider();
      buffer[3] = constrain(-linear_x,-MAX_POWER, MAX_POWER)/ getSpeedDivider();
      buffer[4] = constrain(-linear_x, -MAX_POWER, MAX_POWER)/ getSpeedDivider();
       
        }

    if(angular_z > 0){
      buffer[1] = constrain(angular_z, -MAX_POWER, MAX_POWER) / getSpeedDivider();
      buffer[2] = constrain(angular_z, -MAX_POWER, MAX_POWER) / getSpeedDivider();
      buffer[3] = constrain(angular_z, -MAX_POWER, MAX_POWER) / getSpeedDivider();
      buffer[4] = constrain(-angular_z, -MAX_POWER, MAX_POWER) / getSpeedDivider();
      }else{
        if(angular_z < 0){
      buffer[1] = constrain(angular_z, -MAX_POWER, MAX_POWER) ;
      buffer[2] = constrain(angular_z, -MAX_POWER, MAX_POWER) / getSpeedDivider();
      buffer[3] = constrain(angular_z, -MAX_POWER, MAX_POWER);
      buffer[4] = constrain(-angular_z+10, -MAX_POWER, MAX_POWER) / getSpeedDivider();
        }
       //buffer[3] = constrain(angular_z-5, -MAX_POWER, MAX_POWER) / getSpeedDivider(); 
        }
       
    if(linear_y>0){
      buffer[1] = constrain(linear_y, -MAX_POWER, MAX_POWER) / getSpeedDivider();
      buffer[2] = constrain(linear_y, -MAX_POWER, MAX_POWER) / getSpeedDivider();
      buffer[3] = constrain(-linear_y, -MAX_POWER, MAX_POWER) / getSpeedDivider();
      buffer[4] = constrain(linear_y, -MAX_POWER, MAX_POWER) / getSpeedDivider();
      }else{
        if(linear_y <0){
      buffer[1] = constrain(linear_y, -MAX_POWER, MAX_POWER) / getSpeedDivider();
      buffer[2] = constrain(linear_y, -MAX_POWER, MAX_POWER) / getSpeedDivider();
      buffer[3] = constrain(-linear_y, -MAX_POWER, MAX_POWER) / getSpeedDivider();
      buffer[4] = constrain(linear_y+10, -MAX_POWER, MAX_POWER) / getSpeedDivider();
        }
        }
   
    buffer[5] = getVerticalPower(-linear_z);
    buffer[6] = getVerticalPower(-linear_z);
    buffer[7] = getCamera();
    buffer[8] = getManipulator();
    buffer[9] = END_BYTE;

    SERIAL_CONTROL.write(buffer, 10);

    SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print((int8_t)buffer[2]); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print((int8_t)buffer[1]); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print((int8_t)buffer[3]); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print((int8_t)buffer[4]); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print((int8_t)buffer[5]); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print((int8_t)buffer[6]); SERIAL_DEBUG.print("\t");
    SERIAL_DEBUG.print(" / "); SERIAL_DEBUG.println(getSpeedDivider());

    delay(50);
}
