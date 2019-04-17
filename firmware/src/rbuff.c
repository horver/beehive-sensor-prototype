#include "rbuff.h"
#include "delay.h"
#include <stddef.h>

void rbuff_init(rbuff_t* buffer)
{
	buffer->head = 0;
	buffer->tail = 0;
}

uint8_t rbuff_isfull(rbuff_t* buffer)
{
	if (buffer == NULL)
		return 0;

	uint32_t next = (uint32_t)(buffer->head + 1);
	if (next >= RBUFF_SIZE)
		next = 0;

	return (next == buffer->tail);
}

uint8_t rbuff_isempty(rbuff_t* buffer)
{
	if (buffer == NULL)
		return 0;

	return (buffer->head == buffer->tail);
}

void rbuff_push(rbuff_t* buffer, uint32_t data)
{
	if (buffer == NULL)
		return;

	if (rbuff_isfull(buffer))
		return;

	buffer->buff[buffer->head] = data;

	buffer->head++;

	if (buffer->head >= RBUFF_SIZE)
		buffer->head = 0;

}

uint32_t rbuff_pop(rbuff_t* buffer)
{
	if (buffer == NULL)
		return 0;

	if (rbuff_isempty(buffer))
		return 0;


	uint32_t data = buffer->buff[buffer->tail];

	buffer->tail++;

	if (buffer->tail >= RBUFF_SIZE)
		buffer->tail = 0;

	return data;
}
