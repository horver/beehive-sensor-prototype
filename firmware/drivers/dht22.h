#ifndef __DHT22_H__
#define __DHT22_H__

#include <stdint.h>
#include "em_gpio.h"

// Config DHT22 sensor
#define DHT_PORT 	gpioPortD
#define DHT_PIN		7

typedef struct {
	uint16_t humidity;
	int16_t	 temp;
	uint8_t  checksum;
	uint8_t  data[5];
	GPIO_Port_TypeDef port;
	unsigned int pin;
} dht22_t;

void dht22_init(dht22_t* sensor, GPIO_Port_TypeDef port, unsigned int pin);

uint8_t dht22_checksum(uint8_t* data);

void dht22_read(dht22_t* sensor);
float dht22_humidity(dht22_t* sensor);
float dht22_temp(dht22_t* sensor);


#endif
