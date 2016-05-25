#include <SoftwareSerial.h>
#include "common.h"


SoftwareSerial myBTSerial(BT_PIN_TX, BT_PIN_RX);


void bt_setup()
{
	myBTSerial.begin(BT_BAUD);
}


void bt_loop()
{
  control_action(myBTSerial);
}
