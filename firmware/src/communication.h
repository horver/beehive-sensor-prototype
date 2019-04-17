#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#define COMM_CMD_STS    0x00
#define COMM_CMD_RTH	0x01
#define COMM_CMD_SAS	0x02
#define COMM_CMD_RST	0x06
#define COMM_CMD_FIN	0x07

#define COMM_RES_OK    0x00
#define COMM_RES_ERR   0x01

#define COMM_SAMPLES_64 	0x00 // 64 samples
#define COMM_SAMPLES_128 	0x01 // 128 samples
#define COMM_SAMPLES_256 	0x02 // 256 samples
#define COMM_SAMPLES_512	0x03 // 512 samples

#define COMM_BURSTS_1 	0x00
#define COMM_BURSTS_2 	0x01
#define COMM_BURSTS_4 	0x02
#define COMM_BURSTS_8	0x03
#define COMM_BURSTS_16	0x04
#define COMM_BURSTS_32 	0x05
#define COMM_BURSTS_64 	0x06

#define ADDRESS	0x00

typedef enum comm_state_t {IDLE,			// Listening for STS
						   ANSWER, 			// Transmit status
						   WAIT_TH, 		// Listening for RTH
						   TXTH, 			// Transmit T/H data
						   WAIT_SND, 		// Listening for SAS
						   WAIT_SAMPLES,	// Listening for number of samples
						   WAIT_BURST,		// Listening for number of burst
						   TXSND, 			// Transmit ADC data
						   WAIT_FIN, 		// Listening for FIN
						   SILENTIUM		// Not my turn
} comm_state_t;

#endif
