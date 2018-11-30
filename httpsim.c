#ifndef STDINT
#define STDINT
#include <stdint.h>
#endif

#ifndef STDIO
#define STDIO
#include <stdio.h>
#endif

#include "queue.h"
#include "random_var.h"

//int main (int argc, char** argv)
int main ()
{
	float current_time;

	/* TODO: Read these from arguments */
	uint32_t request_queue_length_max = 50;
	float request_mean_delay = 30.f;
	float mean_service_time = 20.f;
	float max_simulation_time = 5000.f;

	queue request_queue;

	

	return 0;
}
