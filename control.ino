#define CMD_PING	'p'
#define CMD_CONFIG	'c'
#define CMD_STATUS 's'
#define CMD_EXIT	'x'
#define CMD_HELP '?'

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




void control_action(Stream& io)
{
  if(!io.available()) {
    return;
  }
  char ch = io.read();
  
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
		io.println("OK");
		break;
	case CMD_SERVO_CLAW_OPEN:
		servo_to_max(S_CLAW);
		io.println("OK");
		break;

	case CMD_SERVO_NECK_LEFT:
		servo_to_min(S_NECK);
		io.println("OK");
		break;
	case CMD_SERVO_NECK_RIGHT:
		servo_to_max(S_NECK);
		io.println("OK");
		break;

	case CMD_SERVO_NECK_UP:
		servo_to_max(S_NECK_2);
		io.println("OK");
		break;
	case CMD_SERVO_NECK_DOWN:
		servo_to_min(S_NECK_2);
		io.println("OK");
		break;

	case CMD_SERVO_BODY_UP:
		servo_to_max(S_2FACE);
		io.println("OK");
		break;
	case CMD_SERVO_BODY_DOWN:
		servo_to_min(S_2FACE);
		io.println("OK");
		break;

	case CMD_SERVO_BODY_LEFT:
		servo_left(S_BASE);
		io.println("OK");
		break;
	case CMD_SERVO_BODY_RIGHT:
		servo_right(S_BASE);
		io.println("OK");
		break;

	case CMD_PING:
		io.println("pong");
		break;
	case CMD_CONFIG:
		io.println("NONE");
		break;
	case CMD_EXIT:
		io.println("OK");
		system_stop();
		break;
  case CMD_STATUS:
    servos_status(io);
    break;
  case CMD_HELP:
    io.println(":)");
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
