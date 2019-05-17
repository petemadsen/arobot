#define CMD_PING	'p'
#define CMD_CONFIG	'c'
#define CMD_STATUS 's'
#define CMD_EXIT	'x'
#define CMD_HELP '?'

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

#define CMD_EMAGNET_ON 'O'
#define CMD_EMAGNET_OFF 'o'

#define HELP_REPLY_0 "p.ing c.onfig s.tatus e.x.it"
#define HELP_REPLY_1 "qQ: left wW: up eE: up rR: left tT: open"
#define HELP_REPLY_2 "oO: magnet"


#define CMD_BUFFER_LEN 20
static uint8_t cmd_buffer[CMD_BUFFER_LEN + 1];
static uint8_t cmd_buffer_cnt = 0;


void control_action(Stream& io)
{
  while (io.available())
  {
    char ch = io.read();
    if (cmd_buffer_cnt < CMD_BUFFER_LEN)
    {
      cmd_buffer[cmd_buffer_cnt++] = ch;
    }
    control_run_command(ch, io);
    //delay(1000);
  }
#if 0
  if (cmd_buffer_cnt != 0)
  {
    control_run_command(cmd_buffer[cmd_buffer_cnt - 1], io);
    --cmd_buffer_cnt;
    delay(1000);
  }
#endif
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
