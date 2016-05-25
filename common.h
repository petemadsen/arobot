#ifndef __MY_COMMON_H__
#define __MY_COMMON_H__


#define SERVO_PIN_CLAW		7
#define SERVO_PIN_NECK		6
#define SERVO_PIN_NECK_2	5
#define SERVO_PIN_2FACE		4
#define SERVO_PIN_2FACE2	3
#define SERVO_PIN_BASE		2


#define DEBUG_SERVO_POS(x,y,z) Serial.print("S_TO["); Serial.print(x); Serial.print("]: "); Serial.print(y); Serial.print(" to "); Serial.println(z);


#define BT_PIN_TX	13
#define BT_PIN_RX	12
#define BT_BAUD		9600


#define NR_SERVOS  6
#define S_BASE    0
#define S_2FACE   1
#define S_2FACE2  2
#define S_CLAW    3
#define S_NECK    4
#define S_NECK_2  5


#endif
