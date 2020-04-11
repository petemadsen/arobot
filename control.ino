#define CMD_PING	'p'
#define CMD_CONFIG	'c'
#define CMD_STATUS 'y'
#define CMD_EXIT	'x'
#define CMD_HELP '?'
#define CMD_BT_PING 'b'

// qwertzu?
#define CMD_SERVO_BODY_LEFT	'q'
#define CMD_SERVO_BODY_RIGHT	'a'
#define CMD_SERVO_BODY_UP	'w'
#define CMD_SERVO_BODY_DOWN	's'
#define CMD_SERVO_NECK_UP	'e'
#define CMD_SERVO_NECK_DOWN	'd'
#define CMD_SERVO_NECK_LEFT	'r'
#define CMD_SERVO_NECK_RIGHT	'f'
#define CMD_SERVO_CLAW_CLOSE	't'
#define CMD_SERVO_CLAW_OPEN	'g'

#define CMD_EMAGNET_ON 'z'
#define CMD_EMAGNET_OFF 'h'

#define HELP_REPLY_0 "p.ing c.onfig y:status e.x.it"
#define HELP_REPLY_1 "body qa: left/right\nbody ws: up/down\nhead ed: up/down\nhead rf: left/right\nhead tg: open/close"
#define HELP_REPLY_2 "magnet zh: on/off"


static bool bt_ping = false;


void control_action(Stream& io)
{
  while (io.available())
  {
    char ch = io.read();
    control_run_command(ch, io);
  }
}

static void control_run_command(char ch, Stream& io)
{
	switch (ch)
	{
	case CMD_SERVO_CLAW_CLOSE:
		servo_left(S_CLAW);
		io.println("OK");
		break;
	case CMD_SERVO_CLAW_OPEN:
		servo_right(S_CLAW);
		io.println("OK");
		break;

	case CMD_SERVO_NECK_LEFT:
		servo_left(S_NECK_LR);
		io.println("OK");
		break;
	case CMD_SERVO_NECK_RIGHT:
		servo_right(S_NECK_LR);
		io.println("OK");
		break;

	case CMD_SERVO_NECK_UP:
		servo_left(S_NECK_2);
		io.println("OK");
		break;
	case CMD_SERVO_NECK_DOWN:
		servo_right(S_NECK_2);
		io.println("OK");
		break;

	case CMD_SERVO_BODY_UP:
		servo_left(S_2FACE);
		io.println("OK");
		break;
	case CMD_SERVO_BODY_DOWN:
		servo_right(S_2FACE);
		io.println("OK");
		break;

	case CMD_SERVO_BODY_LEFT:
		servo_left(S_BASE);
    //servo_to_min(S_BASE);
		io.println("OK");
		break;
	case CMD_SERVO_BODY_RIGHT:
		servo_right(S_BASE);
    //servo_to_max(S_BASE);
		io.println("OK");
		break;

  case CMD_EMAGNET_ON:
    digitalWrite(EMAGNET_PIN, HIGH);
    io.println("OK");
    break;
  case CMD_EMAGNET_OFF:
    digitalWrite(EMAGNET_PIN, LOW);
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
	  io.println(HELP_REPLY_0);
	  io.println(HELP_REPLY_1);
	  io.println(HELP_REPLY_2);
    io.println(":)");
    servos_status(io);
    break;

  case CMD_BT_PING:
    bt_ping = true;
    break;

	default:
		ch = 0;
	}

	if (ch) {
		Serial.print("CMD: ");
		Serial.print(ch);
		Serial.println();
	}
}

bool control_test_bt()
{
  bool b = bt_ping;
  bt_ping = false;
  return b;
}
