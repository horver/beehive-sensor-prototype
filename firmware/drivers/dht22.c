#include "dht22.h"

#include "em_cmu.h"
#include "em_device.h"
#include "em_timer.h"
#include "em_int.h"

#include "../src/delay.h"

#include <stddef.h>

void dht22_init(dht22_t* sensor, GPIO_Port_TypeDef port, unsigned int pin)
{
	if (sensor == NULL)
		return;

	CMU_ClockEnable(cmuClock_TIMER1, true);

	sensor->humidity = 0;
	sensor->temp = 0;
	sensor->checksum = 0;
	sensor->port = port;
	sensor->pin = pin;
}

void dht22_read(dht22_t* sensor)
{

	if (sensor == NULL)
		return;

	// Disable interrupts
	INT_Disable();

	uint8_t received[5];
	uint8_t mask = 128;
	uint8_t byte = 4;
	uint8_t checksum;

	for (uint8_t i = 5; i; --i)
		received[i-1] = 0;

	// Send start signal
	GPIO_PinModeSet(sensor->port, sensor->pin, gpioModePushPull, 1);
	GPIO_PinOutClear(sensor->port, sensor->pin);
	delayMs(1);

	// Release the bus
	GPIO_PinModeSet(sensor->port, sensor->pin, gpioModeInputPull, 0);
	delayUs(28);

	// Wait for response
	while (!GPIO_PinInGet(sensor->port, sensor->pin)) ;
	while (GPIO_PinInGet(sensor->port, sensor->pin)) ;

	// Receiving 40 bits
	for (uint8_t i = 40; i; --i)
	{
		// Bit start
		while (!GPIO_PinInGet(sensor->port, sensor->pin)) ;

		TIMER1->CNT = 0;
		TIMER1->CMD = TIMER_CMD_START;
		while (GPIO_PinInGet(sensor->port, sensor->pin)) ;
		TIMER1->CMD = TIMER_CMD_STOP;

 		if ((TIMER1->CNT/48) > 10)
 			received[byte] |= mask;

		mask = mask >> 1;

		if (!mask)
		{
			mask = 128;
			byte--;
		}
	}

	// Enable interrupts
	INT_Enable();


	checksum = (received[4]+received[3]+received[2]+received[1])&0xff;

	if (checksum != received[0])
		return;

	sensor->humidity = (uint16_t)((received[4]<<8) | received[3]);
	sensor->temp = (int16_t)((received[2]<<8) | received[1]);
	sensor->checksum = received[0];
	// For test
	sensor->data[4] = received[4];
	sensor->data[3] = received[3];
	sensor->data[2] = received[2];
	sensor->data[1] = received[1];
	sensor->data[0] = received[0];

}

float dht22_humidity(dht22_t* sensor)
{
	if (sensor == NULL)
		return 0;

	return (float)(sensor->humidity/10);
}

float dht22_temp(dht22_t* sensor)
{
	if (sensor == NULL)
		return 0;

	return (float)(sensor->temp/10);
}
