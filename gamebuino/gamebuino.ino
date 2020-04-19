#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

#include <Wire.h>



#define I2C_CLIENT_ADDR 8


// the setup routine runs once when Gamebuino starts up
void setup()
{
  Serial.begin(9600);
  
  gb.begin();
  //gb.battery.show = true;
  //display the main menu:
  gb.titleScreen(F("My arobot game"));
  //gb.popup(F("Let's go!"), 100);


  Wire.begin();
}


void loop()
{
	if (gb.update())
	{      
		gb.display.println(F("Hello World!"));
		int count;
		count = gb.frameCount;
		gb.display.println(count);

		char sendme = ' ';
		if (gb.buttons.pressed(BTN_LEFT))
			sendme = 'q';
		else if (gb.buttons.pressed(BTN_RIGHT))
			sendme = 'a';
		else if (gb.buttons.pressed(BTN_DOWN))
			sendme = 'w';
		else if (gb.buttons.pressed(BTN_UP))
			sendme = 's';
		else if (gb.buttons.pressed(BTN_A))
			sendme = 'z';
		else if (gb.buttons.pressed(BTN_B))
			sendme = 'h';

		if (sendme != ' ')
		{
			Wire.beginTransmission(I2C_CLIENT_ADDR);
			Wire.write(sendme);
			Wire.write("\n");
			Wire.endTransmission();
		}

#if 0
		int i2c_found = 0;
		for (int addr = 0x0; addr < 0x10; ++addr)
		{
			Wire.beginTransmission(addr);
			byte error = Wire.endTransmission();
			if (error == 0)
			{
				i2c_found += 1;
				gb.display.print(F("dev: "));
				gb.display.println(addr);
				//Serial.print(F("Found: "));
				//Serial.println(addr, HEX);
			}
		}
		gb.display.print(F("i2c found: "));
		gb.display.println(i2c_found);
		Serial.println(F("<<done"));
#endif
	}
}
