#include <SoftwareSerial.h>
#include "common.h"


static SoftwareSerial my_bt_serial(BT_PIN_TX, BT_PIN_RX);


void bt_setup()
{
	my_bt_serial.begin(BT_BAUD);
}


void bt_loop()
{
  if (control_test_bt())
    my_bt_serial.println("[bping]");
  
#if 1
  if (my_bt_serial.available())
    Serial.println("[bt]");
  control_action(my_bt_serial);
#else
  while (my_bt_serial.available())
  {
    int ch = my_bt_serial.read();
    Serial.print("--bt: ");
    Serial.println(ch);
  }
#endif
}
