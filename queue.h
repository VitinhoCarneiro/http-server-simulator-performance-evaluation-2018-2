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
	void *data;
	uint32_t q_start;
	uint32_t q_length;
	uint32_t max_q_length;
	size_t unit_size;
} queue;

/* ----------------------------------------------------------
 * Allocates a queue with a given unit_size and max_length. |
 * Returns NULL if the queue cannot be allocated.           |
 * - max_length must be under 2^31                          |
 * ------------------------------------------------------- */
queue *queue_allocate (size_t unit_size, uint32_t max_length);

/* --------------------------------------------------
 * Reallocates a queue with the given max_length.   |
 * Returns NULL if the queue cannot be reallocated. |
 * - new_max_length must be under 2^31              |
 * - new_max_length must not be less than the       |
 *   number of elements currently in the queue.     |
 * ----------------------------------------------- */
queue *queue_reallocate (queue *q, uint32_t new_max_length);

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
