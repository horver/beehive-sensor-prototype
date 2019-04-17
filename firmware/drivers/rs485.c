#include "rs485.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_leuart.h"
#include "../src/delay.h"

void rs485_init(void)
{
	// Setup LEUART
	CMU_ClockEnable(cmuClock_CORELE, true);
	CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
	CMU_ClockEnable(cmuClock_LEUART0, true);

	GPIO_PinModeSet(RS485_UART_PORT, RS485_TX_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(RS485_UART_PORT, RS485_RX_PIN, gpioModeInputPull, 1);

	LEUART_Init_TypeDef leuart_init = {
		.baudrate = 9600,
		.databits = leuartDatabits8,
		.parity	  = leuartNoParity,
		.stopbits = leuartStopbits1,
		.enable   = leuartEnable,
		.refFreq  = 0
	};

	LEUART_Init(LEUART0, &leuart_init);

	LEUART0->ROUTE |= LEUART_ROUTE_LOCATION_LOC0 | LEUART_ROUTE_TXPEN | LEUART_ROUTE_RXPEN;

	LEUART_IntEnable(LEUART0, LEUART_IEN_RXDATAV);
	//LEUART_IntEnable(LEUART0, LEUART_IEN_TXC);
	NVIC_EnableIRQ(LEUART0_IRQn);

	// RS485 control pins
	GPIO_PinModeSet(RS485_CNTR_PORT, RS485_RE_PIN, gpioModePushPull, 0);	// Listening
	GPIO_PinModeSet(RS485_CNTR_PORT, RS485_DE_PIN, gpioModePushPull, 0);

	//LEUART_Enable(LEUART0, leuartEnable);
}

void rs485_setmode(RS485_MODE_t mode)
{
	delayUs(RS485_WAIT_US);
	if (mode == RS485_MODE_TRANSMITTING)
	{
		GPIO_PinOutSet(RS485_CNTR_PORT, RS485_DE_PIN);
		GPIO_PinOutSet(RS485_CNTR_PORT, RS485_RE_PIN);
	}

	if (mode == RS485_MODE_RECEIVING)
	{
		GPIO_PinOutClear(RS485_CNTR_PORT, RS485_DE_PIN);
		GPIO_PinOutClear(RS485_CNTR_PORT, RS485_RE_PIN);
	}
	delayUs(RS485_WAIT_US);
}

