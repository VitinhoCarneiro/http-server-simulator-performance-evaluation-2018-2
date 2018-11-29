#ifndef STDINT
#define STDINT
#include <stdint.h>
#endif

#ifndef STDLIB
#define STDLIB
#include <stdlib.h>
#endif

typedef struct __queue
{
	void **data;
	uint32_t q_start;
	uint32_t q_length;
	uint32_t max_q_length;
} queue;

/* -------------------------------------------------
 * Allocates a queue with a given max_length.
 * Returns NULL if the queue cannot be allocated.
 * ---------------------------------------------- */
queue *queue_allocate (uint32_t max_length);

/* ------------------------------------------------------------------
 * Pops the element in the end of the queue and returns it.
 * Returns NULL if the queue is empty.
 * (it's your problem if you're storing null objects in the queue) 
 * --------------------------------------------------------------- */
void *queue_pop (queue *q);

/* ---------------------------------------------------------------------------------
 * Pushes a given element into the queue.
 * Returns 0 if the queue is full, or the queue length after insertion otherwise.
 * ------------------------------------------------------------------------------ */
int queue_push (queue *q, void *dptr);

/* Frees the queue. */
queue *queue_free (queue *q);
