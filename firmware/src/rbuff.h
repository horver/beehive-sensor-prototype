#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include <stdint.h>

#define RBUFF_SIZE	512

typedef struct rbuff_t {
	uint32_t head;
	uint32_t tail;
	uint32_t buff[RBUFF_SIZE];
} rbuff_t ;

void rbuff_init(rbuff_t* buffer);

uint8_t rbuff_isfull(rbuff_t* buffer);
uint8_t rbuff_isempty(rbuff_t* buffer);

void rbuff_push(rbuff_t* buffer, uint32_t data);
uint32_t rbuff_pop(rbuff_t* buffer);

#endif
