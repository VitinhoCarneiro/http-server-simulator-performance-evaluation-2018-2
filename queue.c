#include "queue.h"
#include <string.h>

#ifndef STDINT
#define STDINT
#include <stdint.h>
#endif

queue *queue_allocate (size_t unit_size, uint32_t max_length)
{
	if (max_length >= (uint32_t) (1 << 31)) return NULL;
	queue *q = malloc(sizeof(queue));
	if (!q) return NULL;
	q->data = malloc(unit_size * max_length);
	if (!q->data)
	{
		free(q);
		return NULL;
	}
	q->q_start = 0;
	q->q_length = 0;
	q->max_q_length = max_length;
	q->unit_size = unit_size;
	return q;
}

queue *queue_reallocate (queue *q, uint32_t new_max_length)
{
	if (!q || !q->data) return NULL;
	if (new_max_length >= (uint32_t) (1 << 31)) return NULL;
	if (new_max_length < q->q_length) return NULL;
	void *new_heap = realloc(q->data, q->unit_size * new_max_length);
	if (!new_heap) return NULL;
	q->data = new_heap;
	/* The hack below saves tons of time in certain cases */
	if (q->q_start == 0 && new_max_length > q->max_q_length) q->q_start = q->max_q_length;
	/* Move memory around if necessary */
	if (q->q_length > q->q_start)
	{
		memmove(q->data + q->unit_size * 
				(((q->max_q_length + q->q_start - q->q_length - 1) 
				  % q->max_q_length) 
				 + (new_max_length - q->max_q_length)), 
			q->data + q->unit_size * 
				((q->max_q_length + q->q_start - q->q_length - 1) 
				 % q->max_q_length), 
			q->unit_size * (q->max_q_length -
				((q->max_q_length + q->q_start - q->q_length - 1) 
				 % q->max_q_length)));
	}
	q->max_q_length = new_max_length;
	return q;
}

void *queue_pop (queue *q)
{
	if (q->q_length == 0) return NULL;
	q->q_length -= 1;
	return q->data + q->unit_size * ((q->max_q_length + q->q_start - q->q_length - 1) % q->max_q_length);
}

void *queue_index (queue *q, int32_t index)
{
	if (index >= 0)
		return q->data + q->unit_size * ((q->max_q_length + q->q_start - q->q_length + index) % q->max_q_length);
	return q->data + q->unit_size * ((q->max_q_length + q->q_start - (-index % q->max_q_length)) % q->max_q_length);
}

int queue_push (queue *q, void *dptr)
{
	if (q->q_length >= q->max_q_length) return 0;
	memcpy(q->data + q->unit_size * q->q_start, dptr, q->unit_size);
	q->q_start = (q->q_start + 1) % q->max_q_length;
	q->q_length += 1;
	return q->q_length;
}

queue *queue_free (queue *q)
{
	if (!q) return NULL;
	free(q->data);
	free(q);
	return q;
}
