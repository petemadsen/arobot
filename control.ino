#define CMD_PING	'p'
#define CMD_CONFIG	'c'
#define CMD_STATUS 's'
#define CMD_EXIT	'x'

#define CMD_BASE_LEFT	'b'
#define CMD_BASE_RIGHT	'B'

// qwertzu?
#define CMD_SERVO_BODY_LEFT	'q'
#define CMD_SERVO_BODY_RIGHT	'Q'
#define CMD_SERVO_BODY_UP	'w'
#define CMD_SERVO_BODY_DOWN	'W'
#define CMD_SERVO_NECK_UP	'e'
#define CMD_SERVO_NECK_DOWN	'E'
#define CMD_SERVO_NECK_LEFT	'r'
#define CMD_SERVO_NECK_RIGHT	'R'
#define CMD_SERVO_CLAW_CLOSE	't'
#define CMD_SERVO_CLAW_OPEN	'T'




void control_action(char ch)
{
  
	switch(ch)
	{
	case CMD_BASE_LEFT:
		servo_left(S_BASE);
		break;
	case CMD_BASE_RIGHT:
		servo_right(S_BASE);
		break;

	case CMD_SERVO_CLAW_CLOSE:
		servo_to_min(S_CLAW);
		myBTSerial.println("OK");
		break;
	case CMD_SERVO_CLAW_OPEN:
		servo_to_max(S_CLAW);
		myBTSerial.println("OK");
		break;

	case CMD_SERVO_NECK_LEFT:
		servo_to_min(S_NECK);
		myBTSerial.println("OK");
		break;
	case CMD_SERVO_NECK_RIGHT:
		servo_to_max(S_NECK);
		myBTSerial.println("OK");
		break;

	case CMD_SERVO_NECK_UP:
		servo_to_max(S_NECK_2);
		myBTSerial.println("OK");
		break;
	case CMD_SERVO_NECK_DOWN:
		servo_to_min(S_NECK_2);
		myBTSerial.println("OK");
		break;

	case CMD_SERVO_BODY_UP:
		servo_to_max(S_2FACE);
		myBTSerial.println("OK");
		break;
	case CMD_SERVO_BODY_DOWN:
		servo_to_min(S_2FACE);
		myBTSerial.println("OK");
		break;

	case CMD_SERVO_BODY_LEFT:
		servo_to_max(S_BASE);
		myBTSerial.println("OK");
		break;
	case CMD_SERVO_BODY_RIGHT:
		servo_to_min(S_BASE);
		myBTSerial.println("OK");
		break;

	case CMD_PING:
		myBTSerial.println("pong");
		break;
	case CMD_CONFIG:
		myBTSerial.println("NONE");
		break;
	case CMD_EXIT:
		myBTSerial.println("OK");
		system_stop();
		break;
	default:
		ch = 0;
	}

	if(ch) {
		Serial.print("CMD: ");
		Serial.print(ch);
		Serial.println();
	}
}
