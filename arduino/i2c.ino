#include <Wire.h>



#define I2C_MAX_BUF 80
static char i2c_buffer[I2C_MAX_BUF];
static uint8_t i2c_buffer_len = 0;



void i2c_setup()
{
	Wire.begin(I2C_ADDR);
	Wire.onReceive(i2c_recv);
}


void i2c_recv(int num_bytes)
{
	while (Wire.available())
	{
		if (i2c_buffer_len < I2C_MAX_BUF)
		{
			i2c_buffer[i2c_buffer_len++] = Wire.read();
		}
	}
}


void i2c_loop()
{
	for (uint8_t i = 0; i < i2c_buffer_len; ++i)
		Serial.print(i2c_buffer[i]);
	i2c_buffer_len = 0;
}
