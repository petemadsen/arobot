#include <Servo.h>

Servo my_servos[NR_SERVOS];

struct servo_pos_t {
  int min;
  int max;
  int now;
  int wanted;
};
struct servo_pos_t servo_pos[NR_SERVOS];

 
#define MAX_POS 160
#define MIN_POS 20
#define END_POS 93

#define SERVOS_LOOP_INTERVAL 30
unsigned long servos_last_access = 0;


void servos_setup()
{
  for (int i=0; i<NR_SERVOS; ++i)
  {
    servo_pos[i].min = MIN_POS;
    servo_pos[i].max = MAX_POS;
    servo_pos[i].now = END_POS;
    servo_pos[i].wanted = END_POS;
  }
  
  my_servos[S_BASE].attach(SERVO_PIN_BASE);
  my_servos[S_2FACE].attach(SERVO_PIN_2FACE);
  my_servos[S_2FACE2].attach(SERVO_PIN_2FACE2);
  my_servos[S_CLAW].attach(SERVO_PIN_CLAW);
  my_servos[S_NECK_LR].attach(SERVO_PIN_NECK);
  my_servos[S_NECK_2].attach(SERVO_PIN_NECK_2);

  servo_pos[S_BASE].min = 40;
  servo_pos[S_BASE].max = 140;

  servo_pos[S_2FACE].min = 40;
  servo_pos[S_2FACE].max = 140;

  servo_pos[S_CLAW].min = 60;
  servo_pos[S_CLAW].max = 170;

  servo_pos[S_NECK_LR].min = 40;
  servo_pos[S_NECK_LR].max = 140;

  servo_pos[S_NECK_2].min = 40;
  servo_pos[S_NECK_2].max = 140;
}


void servos_loop()
{
  if ((unsigned long)(millis() - servos_last_access) < SERVOS_LOOP_INTERVAL)
  {
    return;
  }
  servos_last_access = millis();
  
  for (int i=0; i<NR_SERVOS; ++i)
  {
    if (servo_pos[i].now != servo_pos[i].wanted)
    {
      Serial.print(i);
      Serial.print(" - moving from ");
      Serial.print(servo_pos[i].now);
      Serial.print(" to ");
      Serial.print(servo_pos[i].wanted);
      Serial.println();

      int pos = servo_pos[i].now + (servo_pos[i].wanted < servo_pos[i].now ? -1 : +1);
      servo_set_to(i, pos);
    }
  }
}

void servos_status(Stream& io)
{
  for(int i=0; i<NR_SERVOS; ++i) {
    io.print(i);
    io.print("=");
    io.print(servo_pos[i].min);
    io.print(",");
    io.print(servo_pos[i].max);
    io.print(",");
    io.print(servo_pos[i].now);
    io.print(",");
    io.print(servo_pos[i].wanted);
    io.print(" ");
  }
  io.println();
}


void system_powerdown()
{
  for(int i=0; i<NR_SERVOS; ++i) {
    my_servos[i].detach();
  }
}




void system_stop()
{
  for(int i=0; i<NR_SERVOS; ++i) {
    servo_pos[i].wanted = END_POS;
  }
}


void servo_to_max(int id)
{
  Serial.print(id);
  Serial.println(" to max");
  servo_pos[id].wanted = servo_pos[id].max;
}


void servo_to_min(int id)
{
  Serial.print(id);
  Serial.println("to min");
  servo_pos[id].wanted = servo_pos[id].min;
}


void servo_set_to(int id, int pos)
{
  if(id==S_2FACE2) {
    return;
  }

  if(id == S_2FACE)
  {
    servo_pos[S_2FACE2].now = pos;
    my_servos[S_2FACE2].write(pos);
  }
  servo_pos[id].now = pos;
  my_servos[id].write(pos);
}


void servo_left(int id)
{
  if (id==S_2FACE2) {
    return;
  }
  
  int new_pos = servo_pos[id].now - 1;
  new_pos = servo_pos[id].wanted - 1;
  DEBUG_SERVO_POS(id, -1, new_pos);
  if (new_pos > servo_pos[id].min)
  {
    servo_pos[id].wanted = new_pos;
    if (id==S_2FACE) {
      servo_pos[S_2FACE2].wanted = new_pos;
    }
  }
}


void servo_right(int id)
{
  if (id==S_2FACE2) {
    return;
  }
  
  int new_pos = servo_pos[id].now + 1;
  new_pos = servo_pos[id].wanted + 1;
  DEBUG_SERVO_POS(id, -1, new_pos);
  if (new_pos < servo_pos[id].max) {
    servo_pos[id].wanted = new_pos;
    if (id==S_2FACE) {
      servo_pos[S_2FACE2].wanted = new_pos;
    }
  }

}


