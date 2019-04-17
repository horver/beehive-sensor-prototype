#include <stdint.h>
#include <stdbool.h>

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_leuart.h"
#include "em_int.h"
#include "em_adc.h"
#include "em_timer.h"

#include "segmentlcd.h"
#include "../drivers/dht22.h"
#include "../drivers/rs485.h"

#include "delay.h"
#include "communication.h"
#include "rbuff.h"

// Config LED indicator
#define USR_LED_PORT 	gpioPortC
#define USR_LED_PIN		4

// Config ADC
#define ADC_PORT	gpioPortD
#define ADC_PIN		6
#define ADC_CH		6
#define ADC_INPUT	adcSingleInpCh6

#define FS			5000	// Sampling frequency
#define TIMER_DIV	(12000000/FS-1)

volatile comm_state_t comm_state = IDLE;
volatile uint32_t sample_cnt = 0;
volatile uint32_t sample_max = RBUFF_SIZE;
volatile uint8_t burst = 1;

volatile rbuff_t sample_buffer;

void LEUART0_IRQHandler(void)
{
	uint8_t received = LEUART0->RXDATA;
	uint8_t command = received&0x07;
	uint8_t address = received&0xf8;

	if (address != ADDRESS)
	{
		if (command == COMM_CMD_FIN)
			comm_state = IDLE;
		else
			comm_state = SILENTIUM;
		return;
	}

	if (address == ADDRESS)
	{

		if (comm_state == WAIT_SAMPLES)
		{
			switch (command)
			{
				case COMM_SAMPLES_64:
					sample_max = 64;
					break;
				case COMM_SAMPLES_128:
					sample_max = 128;
					break;
				case COMM_SAMPLES_256:
					sample_max = 256;
					break;
				case COMM_SAMPLES_512:
					sample_max = 512;
					break;
				default:
					sample_max = RBUFF_SIZE;
					break;
			}
			comm_state = WAIT_BURST;
			return;
		}

		if (comm_state == WAIT_BURST)
		{
			switch (command)
			{
				case COMM_BURSTS_1:
					burst = 1;
					break;
				case COMM_BURSTS_2:
					burst = 2;
					break;
				case COMM_BURSTS_4:
					burst = 4;
					break;
				case COMM_BURSTS_8:
					burst = 8;
					break;
				case COMM_BURSTS_16:
					burst = 16;
					break;
				case COMM_BURSTS_32:
					burst = 32;
					break;
				case COMM_BURSTS_64:
					burst = 64;
					break;
				default:
					burst = 1;
					break;
			}
			TIMER_Enable(TIMER2, true);
			comm_state = TXSND;
			rs485_setmode(RS485_MODE_TRANSMITTING);
			return;
		}

		// Process other commands
		switch (command)
		{
			case COMM_CMD_STS:
				if (comm_state == IDLE)
					comm_state = ANSWER;
				break;
			case COMM_CMD_RTH:
				if (comm_state == WAIT_TH)
					comm_state = TXTH;
				break;
			case COMM_CMD_SAS:
				if (comm_state == WAIT_SND)
				{
					comm_state = WAIT_SAMPLES;
				}
				break;
			case COMM_CMD_RST:
				comm_state = IDLE;
				break;
			case COMM_CMD_FIN:
				if (comm_state == WAIT_FIN)
					comm_state = IDLE;
				break;
			default: break;
		}
	}
}

void TIMER2_IRQHandler(void)
{
	uint32_t adc_data = ADC_DataSingleGet(ADC0);
	ADC_Start(ADC0, ADC_CMD_SINGLESTART);

	if (!rbuff_isfull(&sample_buffer))
	{
		rbuff_push(&sample_buffer, adc_data);
		sample_cnt++;
	}

	TIMER_IntClear(TIMER2, TIMER_IFC_OF);

	if (sample_cnt > (burst*sample_max))
	{
		TIMER_Enable(TIMER2, false);
	}

}

void adc_init(void)
{
	CMU_ClockEnable(cmuClock_ADC0, true);

	ADC_Init_TypeDef adc_init = ADC_INIT_DEFAULT;

	adc_init.warmUpMode = adcWarmupKeepADCWarm;
	adc_init.timebase = 16;

	ADC_Init(ADC0, &adc_init);

	ADC_InitSingle_TypeDef adc_singleinit = ADC_INITSINGLE_DEFAULT;

	adc_singleinit.reference = adcRefVDD;
	adc_singleinit.input = adcSingleInputCh6;

	ADC_InitSingle(ADC0, &adc_singleinit);


	// Setup TIMER
	CMU_ClockEnable(cmuClock_TIMER2, true);
	TIMER_Init_TypeDef timer_init = TIMER_INIT_DEFAULT;

	TIMER_Init(TIMER2, &timer_init);
	TIMER_CounterSet(TIMER2, 0);
	TIMER_TopSet(TIMER2, TIMER_DIV);

	TIMER_IntClear(TIMER2, TIMER_IF_OF);
	TIMER_IntEnable(TIMER2, TIMER_IEN_OF);
	NVIC_EnableIRQ(TIMER2_IRQn);

	TIMER_Enable(TIMER2, false);

}

int main(void)
{
	/* Chip errata */
	CHIP_Init();

	// Setup clock
	CMU_OscillatorEnable(cmuOsc_HFXO, true, true);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	SystemHFXOClockSet(48000000);
	CMU_ClockDivSet(cmuClock_HFPER, cmuClkDiv_4); // 12 MHz

	// For GPIO
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_GPIO, true);

	initDelay();

	// Init RS485
	rs485_init();
	rs485_setmode(RS485_MODE_RECEIVING);

	// Init DHT22 temp and hum sensor
	dht22_t dht_sensor;
	dht22_init(&dht_sensor, DHT_PORT, DHT_PIN);

	// Init ADC
	adc_init();

	rbuff_init(&sample_buffer);

	// Init USR LED
	GPIO_PinModeSet(USR_LED_PORT, USR_LED_PIN, gpioModePushPull, 0);

	uint32_t adc_data;

	while (1)
	{
		switch (comm_state)
		{
			case IDLE:
				GPIO_PinOutClear(USR_LED_PORT, USR_LED_PIN);
				break;
			case ANSWER:
				GPIO_PinOutSet(USR_LED_PORT, USR_LED_PIN);
				rs485_setmode(RS485_MODE_TRANSMITTING);
				LEUART_Tx(LEUART0, (uint8_t)(ADDRESS<<5|COMM_RES_OK));;
				comm_state = WAIT_TH;
				rs485_setmode(RS485_MODE_RECEIVING);
				break;
			case TXTH:
				dht22_read(&dht_sensor);
				rs485_setmode(RS485_MODE_TRANSMITTING);
				LEUART_Tx(LEUART0, dht_sensor.data[4]);
				LEUART_Tx(LEUART0, dht_sensor.data[3]);
				LEUART_Tx(LEUART0, dht_sensor.data[2]);
				LEUART_Tx(LEUART0, dht_sensor.data[1]);
				LEUART_Tx(LEUART0, dht_sensor.data[0]);
				comm_state = WAIT_SND;
				rs485_setmode(RS485_MODE_RECEIVING);
				break;
			case TXSND:
				if (!rbuff_isempty(&sample_buffer))
				{
					adc_data = rbuff_pop(&sample_buffer);
					LEUART_Tx(LEUART0, (uint8_t)(adc_data&0x000000FF));
					LEUART_Tx(LEUART0, (uint8_t)((adc_data&0x0000FF00)>>8));
				} else {
					if (sample_cnt > sample_max)
					{
						while (!rbuff_isempty(&sample_buffer))
						{
							adc_data = rbuff_pop(&sample_buffer);
							LEUART_Tx(LEUART0, (uint8_t)(adc_data&0x000000FF));
							LEUART_Tx(LEUART0, (uint8_t)((adc_data&0x0000FF00)>>8));
						}
						TIMER_Enable(TIMER2, false);
						comm_state = WAIT_FIN;
						sample_cnt = 0;
						rs485_setmode(RS485_MODE_RECEIVING);
					}
				}
				break;
			default: break;
		}
	}
}
