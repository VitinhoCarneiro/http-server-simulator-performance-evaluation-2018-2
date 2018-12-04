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

/* -------------------------------------------------------------------
 * Pops the element in the end of the queue and returns its pointer. |
 * Returns NULL if the queue is empty.                               |
 * OBS: The pointer is not guaranteed to be valid after a queue_push |
 * or queue_reallocate call. Do not use in applications requiring    |
 * thread-safety.                                                    |
 * -----------------------------------------------------------------*/
void *queue_pop (queue *q);

/* -------------------------------------------------------------------
 * Returns a pointer to the element in the given position of the     |
 * queue, from the queue start (oldest element).                     |
 * Use negative numbers to index from the end of the queue           |
 * (-1 is the last, newest element).                                 |
 * OBS: The returned pointer is not guaranteed to be valid after a   |
 * queue_push or queue_reallocate call. Do not use in applications   |
 * requiring thread-safety.                                          |
 * If you index garbage in the inactive area of the queue,           |
 * it's your problem to deal with... check the goddamn queue length. |
 * -----------------------------------------------------------------*/
void *queue_index (queue *q, int32_t index);

/* ----------------------------------------------------------------------
 * Pushes a given element into the queue.                               |
 * Returns 0 if the queue is full, or the queue length after insertion  |
 * otherwise.                                                           |
 * --------------------------------------------------------------------*/
int queue_push (queue *q, void *dptr);

/* Frees the queue. */
queue *queue_free (queue *q);
