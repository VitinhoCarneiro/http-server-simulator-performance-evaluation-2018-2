#include "queue.h"

#ifndef STDINT
#define STDINT
#include <stdint.h>
#endif

_queue *queue_allocate (uint32_t max_length)
{
	if (max_length > (uint32_t) (1 << 31)) return NULL;
	_queue *q = malloc(sizeof(_queue));
	if (!q) return NULL;
	q->data = malloc(sizeof(void*) * max_length);
	if (!q->data)
	{
		free(q);
		return NULL;
	}
	q->q_start = 0;
	q->q_length = 0;
	q->max_q_length = max_length;
	return q;
}

void *queue_pop (_queue *q)
{
	if (q->q_length == 0) return NULL;
	q->q_length -= 1;
	return q->data[(q->max_q_length + q->q_start - q->q_length - 1) % q->max_q_length];
}

int queue_push (_queue *q, void *dptr)
{
	if (q->q_length >= q->max_q_length) return 0;
	q->data[q->q_start] = dptr;
	q->q_start = (q->q_start + 1) % q->max_q_length;
	q->q_length += 1;
	return q->q_length;
}

_queue *queue_free (_queue *q)
{
	if (!q) return NULL;
	free(q->data);
	free(q);
	return q;
}
