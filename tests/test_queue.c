#include "../queue.h"

#ifndef STDINT
#define STDINT
#include <stdint.h>
#endif

#ifndef STDIO
#define STDIO
#include <stdio.h>
#endif

#include <string.h>
#include <errno.h>
int main ()
{
	int i, result, *popped;
	int elements[] = {4, 8, 15, 16, 23, 42};
	
	queue *q = queue_allocate(sizeof(int), 5);
	printf("Queue initialized, q_start = %d, q_length = %d, max_q_length = %d\n", q->q_start, q->q_length, q->max_q_length);
	
	for (i = 0; i < 6; i++)
	{
		result = queue_push(q, &elements[i]);
		if (result) printf("Inserted %d, queue is now length %d/5, q_start = %d.\n", elements[i], q->q_length, q->q_start);
		else printf("Could not insert %d, queue is full.\n", elements[i]);
	}
	printf("Queue elements:\n {");
	for (i = 0; i < 5; i++)
	{
		printf("%d ", *(int*) queue_index(q, i));
	}
	printf("}\n");
	printf("Reallocating array of %p at %p to size 8...\n", q, q->data);
	q = queue_reallocate(q, 8);
	if (!q)
	{
		printf("ERROR: Reallocation failed; queue_reallocate returned NULL (%p): %s.\n", q, strerror(errno));
		exit(1);
	}
	printf("Queue elements:\n {");
	for (i = 0; i < 8; i++)
	{
		printf("%d ", *(int*) queue_index(q, i));
	}
	printf("}\n");
	printf("Queue elements:\n {");
	for (i = 0; i < 8; i++)
	{
		printf("%d ", *(int*) queue_index(q, i - 8));
	}
	printf("}\n");
	for (i = 0; i < 6; i++)
	{
		popped = queue_pop(q);
		if (popped) printf("Popped %d, queue is now length %d/%d, q_start = %d.\n", *popped, q->q_length, q->max_q_length, q->q_start);
		else printf("Could not pop element, queue is empty.\n");
	}
	return 0;
}
