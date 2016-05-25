#include "common.h"

int nr_runs = 2;


void setup()
{ 
	Serial.begin(9600);

  servos_setup();


	bt_setup();
}



void loop() 
{
/*
	if(nr_runs) {
//		servo_to(S_2FACE, MAX_POS);
//		servo_to(S_2FACE, MIN_POS);

//		servo_to(S_BASE, MAX_POS);
//		servo_to(S_BASE, MIN_POS);

//		servo_to(S_CLAW, 145);
//		servo_to(S_CLAW, 50);
//		delay(2000);
    
		if(!--nr_runs) {
//			system_halt();
//			servo_to(S_2FACE, END_POS);
//			servo_to(S_BASE, END_POS);
//			servo_to(S_CLAW, END_POS);
		}
	}
*/

  servos_loop();

	bt_loop();

	while(Serial.available()) {
		control_action(Serial.read());
	}
} 
