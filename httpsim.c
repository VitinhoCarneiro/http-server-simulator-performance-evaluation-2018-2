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

typedef struct _queue_log
{
	uint32_t length;
	float timestamp;
} queue_log;

//int main (int argc, char** argv)
int main ()
{
	/* TODO: Read these from arguments */
	uint32_t request_queue_length_max = 50;
	float request_mean_delay = 30.f;
	float mean_service_time = 20.f;
	float request_timeout = 1200.f;
	float max_simulation_time = 5000.f;

	queue *request_queue = queue_allocate(request_queue_length_max);

	/* Simulation state variables */
	float current_timestamp = 0.f;
	float next_request_arrival = rand_exponential(request_mean_delay);
	int server_is_processing = 0;
	float current_request_processing_end = 0.f;
	// request req_in_service;

	/* Statistic logging variables */
	uint32_t total_requests = 0;
	uint32_t total_queued_requests = 0;
	uint32_t rejected_requests = 0;
	uint32_t served_requests = 0;
	uint32_t discarded_requests = 0;
	// I'm gonna have to queue these below, there's no escape...:
	float total_service_time = 0.f;
	float total_waiting_time = 0.f;
	double accumulated_time_queue_length = 0.;
	
	while (current_timestamp < max_simulation_time)
	{
		if (server_is_processing && current_request_processing_end < next_request_arrival)
		{
			if (current_request_processing_end > max_simulation_time)
			{
				accumulated_time_queue_length += (max_simulation_time - current_timestamp)
				       	* accumulated_time_queue_length;
				break;
			}
			accumulated_time_queue_length += (current_request_processing_end - current_timestamp) * accumulated_time_queue_length;
			served_requests++
			// Update total service time
			/*---------------------------------
			 *  Grab event from queue here    |
			 *  - Decide if going to discard  |
			 *    due to timeout              |
			 *-------------------------------*/
			current_timestamp = current_request_processing_end;
			current_request_processing_end += rand_exponential(mean_service_time);
			continue;
		}
		if (next_request_arrival > max_simulation_time)
		{
			accumulated_time_queue_length += (max_simulation_time - current_timestamp) * accumulated_time_queue_length;
			break;
		}
		accumulated_time_queue_length += (next_request_arrival - current_timestamp) * accumulated_time_queue_length;
		total_requests++;
		/*-----------------------------
		 *  Add event to queue here   |
		 *---------------------------*/
		current_timestamp = next_request_arrival;
		next_request_arrival += rand_exponential(request_mean_delay);
	}

	request_queue = queue_free(request_queue);
	return 0;
}
