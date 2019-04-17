#ifndef __RS485_H__
#define __RS485_H__

#include <stdint.h>

// Config RS485
#define RS485_UART_PORT	gpioPortD
#define RS485_TX_PIN	4
#define RS485_RX_PIN	5
#define RS485_CNTR_PORT gpioPortC
#define RS485_DE_PIN	3
#define RS485_RE_PIN	0

#define RS485_WAIT_US	300

typedef enum RS485_MODE_t {RS485_MODE_TRANSMITTING, RS485_MODE_RECEIVING} RS485_MODE_t;

void rs485_init(void);
void rs485_transmitting(void);
void rs485_setmode(RS485_MODE_t mode);

#endif
