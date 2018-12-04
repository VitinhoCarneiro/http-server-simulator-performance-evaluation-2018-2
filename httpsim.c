#ifndef STDINT
#define STDINT
#include <stdint.h>
#endif

#ifndef STDIO
#define STDIO
#include <stdio.h>
#endif

#include <errno.h>
#include <string.h>
#include <math.h>

#include "queue.h"
#include "random_var.h"

typedef struct _req
{
	double timestamp;
	double waiting_time;
	double service_time;
} http_request;

typedef struct _qlog
{
	double timestamp;
	uint32_t length;
} queue_len_log;

void show_usage (char *bin_name)
{
	fprintf(stderr, "\
Usage: %s (options)\n\
Valid options are as follows:\n\
    -rd/--request-delay (time)\n\
        Sets the mean request arrival delay in milliseconds.\n\
    -svt/--service-time (time)\n\
        Sets the mean service time in milliseconds.\n\
    -to/--timeout (time)\n\
        Sets the request timeout time in milliseconds.\n\
    -q/--queue-length (length)\n\
        Sets the request queue max length in milliseconds.\n\
    -t/--time (time)\n\
        Sets the maximum simulation time in SECONDS.\n\
    -s/--seed (seed)\n\
        Sets a custom random seed for the simulation. The seed is also changed according to the input parameters.\n\
    -h/--help\n\
        Show this help.\n\
", bin_name);
}

void try_enqueue (queue **q, void *dptr)
{
	if (!queue_push(*q, dptr))
	{
		queue *bigger_q = queue_reallocate(*q, (uint32_t) ((*q)->max_q_length * 1.5));
		if (!bigger_q)
		{
			fprintf(stderr, "ERROR: Could not allocate memory for queue (%lu->%lu bytes): %s.\n", (*q)->max_q_length * (*q)->unit_size, (uint64_t) ((*q)->max_q_length * (*q)->unit_size * 1.5), strerror(errno));
			exit(1);
		}
		*q = bigger_q;
		if (!queue_push(*q, dptr))
		{
			fprintf(stderr, "ERROR: Could not append to queue (%p, %u/%u entries of size %lu B).\n", *q, (*q)->q_length, (*q)->max_q_length, (*q)->unit_size);
			exit(1);
		}
	}
}

int main (int argc, char** argv)
{
	uint32_t request_queue_length_max = 50;
	double request_mean_delay = 15.;
	double mean_service_time = 20.;
	double request_timeout = 1200.;
	double max_simulation_time = 5.;
	uint32_t seed = 1;
	int arg;

	for (arg = 1; arg < argc; arg++)
	{
		if (arg + 1 < argc)
		{
			if (strcmp(argv[arg], "-rd") == 0 || strcmp(argv[arg], "--request-delay") == 0)
			{
				if (!sscanf(argv[++arg], "%lf", &request_mean_delay))
				{
					fprintf(stderr, "ERROR: Invalid value for option `%s' (mean request delay): `%s'.\nPlease check your command syntax and try again.\n", argv[arg - 1], argv[arg]);
					show_usage(argv[0]);
					exit(1);
				}
			}
			else if (strcmp(argv[arg], "-svt") == 0 || strcmp(argv[arg], "--service-time") == 0)
			{
				if (!sscanf(argv[++arg], "%lf", &mean_service_time))
				{
					fprintf(stderr, "ERROR: Invalid value for option `%s' (mean service time): `%s'.\nPlease check your command syntax and try again.\n", argv[arg - 1], argv[arg]);
					show_usage(argv[0]);
					exit(1);
				}
			}
			else if (strcmp(argv[arg], "-to") == 0 || strcmp(argv[arg], "--timeout") == 0)
			{
				if (!sscanf(argv[++arg], "%lf", &request_timeout))
				{
					fprintf(stderr, "ERROR: Invalid value for option `%s' (request timeout): `%s'.\nPlease check your command syntax and try again.\n", argv[arg - 1], argv[arg]);
					show_usage(argv[0]);
					exit(1);
				}
			}
			else if (strcmp(argv[arg], "-q") == 0 || strcmp(argv[arg], "--queue-length") == 0)
			{
				if (!sscanf(argv[++arg], "%d", &request_queue_length_max))
				{
					fprintf(stderr, "ERROR: Invalid value for option `%s' (maximum request queue length): `%s'.\nPlease check your command syntax and try again.\n", argv[arg - 1], argv[arg]);
					show_usage(argv[0]);
					exit(1);
				}
			}
			else if (strcmp(argv[arg], "-t") == 0 || strcmp(argv[arg], "--time") == 0)
			{
				if (!sscanf(argv[++arg], "%lf", &max_simulation_time))
				{
					fprintf(stderr, "ERROR: Invalid value for option `%s' (max simulation time): `%s'.\nPlease check your command syntax and try again.\n", argv[arg - 1], argv[arg]);
					show_usage(argv[0]);
					exit(1);
				}
			}
			else if (strcmp(argv[arg], "-s") == 0 || strcmp(argv[arg], "--seed") == 0)
			{
				if (!sscanf(argv[++arg], "%d", &seed))
				{
					fprintf(stderr, "ERROR: Invalid value for option `%s' (random seed): `%s'.\nPlease check your command syntax and try again.\n", argv[arg - 1], argv[arg]);
					show_usage(argv[0]);
					exit(1);
				}
			}
			else
			{
				fprintf(stderr, "ERROR: Invalid option `%s'.\n", argv[arg]);				show_usage(argv[0]);
				exit(1);
			}
		}
		else
		{
			if (strcmp(argv[arg], "-h") == 0 || strcmp(argv[arg], "--help") == 0)
			{
				show_usage(argv[0]);
				exit(1);
			}
			fprintf(stderr, "ERROR: Trailing argument in command line: `%s' \nPlease check your command syntax and try again.\n", argv[arg]);
			show_usage(argv[0]);
			exit(1);
		}
	}
	
	max_simulation_time *= 1000;

	uint32_t initial_seed = seed;
	seed = (seed ^ request_queue_length_max)
		^ (uint32_t) (request_mean_delay * 14256)
		^ (uint32_t) (mean_service_time * 91648)
		^ (uint32_t) (request_timeout * 1502)
		^ (uint32_t) (max_simulation_time * 311);
	seed_rng(seed);

	queue *request_queue = queue_allocate(sizeof(http_request), request_queue_length_max);

	/* Simulation state variables */
	double current_timestamp = 0.;
	double next_request_arrival = rand_exponential(request_mean_delay);
	int server_is_processing = 0;
	double current_request_processing_end = 0.;
	http_request req_in_service;

	/* Statistic logging variables */
	queue *request_log = queue_allocate(sizeof(http_request), 256);
	queue *request_queue_log = queue_allocate(sizeof(queue_len_log), 256);

	uint32_t total_requests = 0;
	uint32_t total_queued_requests = 0;
	uint32_t rejected_requests = 0;
	uint32_t served_requests = 0;
	uint32_t discarded_requests = 0;

	uint32_t timestamp_report_delay = 1000, timestamp_report_counter = 1000;
	

	printf("Running simulation for %.3lf seconds - RNG seed is %u, computed simulation seed is %u.\n", max_simulation_time / 1000, initial_seed, seed);
	printf("\
Parameters - mean request delay = %.3lf ms,\n\
              mean service time = %.3lf ms,\n\
                        timeout = %.3lf ms,\n\
           maximum queue length = %u\n", request_mean_delay, mean_service_time, request_timeout, request_queue_length_max);

	while (current_timestamp < max_simulation_time)
	{
		if (--timestamp_report_counter == 0)
		{
			fprintf(stderr, "Current time: %5.3lf s\r", current_timestamp / 1000);
			timestamp_report_counter = timestamp_report_delay;
		}
		if (server_is_processing && current_request_processing_end < next_request_arrival)
		{
			if (current_request_processing_end > max_simulation_time)
			{
				break;
			}
			/* Update statistics */
			req_in_service.service_time = current_request_processing_end - req_in_service.timestamp - req_in_service.waiting_time;
			try_enqueue(&request_log, &req_in_service);
			/*---------------------------------
			 *  Grab event from queue here    |
			 *  - Decide if going to discard  |
			 *    due to timeout              |
			 *-------------------------------*/
			while(1)
			{
				http_request *popped_request = queue_pop(request_queue);
				if (popped_request)
				{
					if (current_request_processing_end > popped_request->timestamp + request_timeout)
					{
						discarded_requests++;
						continue;
					}
					served_requests++;
					memcpy(&req_in_service, popped_request, sizeof(http_request));
					req_in_service.waiting_time = current_request_processing_end - req_in_service.timestamp;
				}
				else
				{
					server_is_processing = 0;
				}
				break;
			}
			/* -- Log queue length change -- */
			queue_len_log dp;
			dp.timestamp = current_request_processing_end;
			dp.length = request_queue->q_length;
			try_enqueue(&request_queue_log, &dp);
			/*-------------------------------*/
			current_timestamp = current_request_processing_end;
			if (server_is_processing)
				current_request_processing_end += rand_exponential(mean_service_time);
			continue;
		}
		if (next_request_arrival > max_simulation_time)
		{
			break;
		}
		total_requests++;
		/*------------------------------------
		 *  Try to add event to queue here   |
		 *----------------------------------*/
		http_request arriving_event;
		arriving_event.timestamp = next_request_arrival;
		arriving_event.waiting_time = -1.;
		arriving_event.service_time = -1.;
		if (server_is_processing)
		{
			if (!queue_push(request_queue, (void*) &arriving_event))
			{
				rejected_requests++;
				//fprintf(stderr, "Rejected request, ts=%lf, req_queue=%d/%d\n", next_request_arrival, request_queue->q_length, request_queue->max_q_length);
			}
			else total_queued_requests++;
			queue_len_log dp;
			dp.timestamp = next_request_arrival;
			dp.length = request_queue->q_length;
			try_enqueue(&request_queue_log, &dp);
		}
		else
		{
			req_in_service = arriving_event;
			req_in_service.waiting_time = 0.;
			server_is_processing = 1;
			current_request_processing_end = next_request_arrival + rand_exponential(mean_service_time);
			total_queued_requests++;
			served_requests++;
		}
		/*----------------------------------*/
		current_timestamp = next_request_arrival;
		next_request_arrival += rand_exponential(request_mean_delay);
	}
	printf("                                           \n");
	printf("Total requests: %u\n", total_requests);
	printf("|-- Total queued requests: %u/%u (%.2lf%%)\n", total_queued_requests, total_requests, 100. * total_queued_requests / total_requests);
	printf("|    |\n|    |-- Served requests: %u/%u (%.2lf%%)\n", served_requests, total_queued_requests, 100. * served_requests / total_queued_requests);
	printf("|    |-- Discarded requests (timeout): %u/%u (%.2lf%%)\n", discarded_requests, total_queued_requests, 100. * discarded_requests / total_queued_requests);
	printf("|    +-- Requests still in queue: %u/%u (%.2lf%%)\n", total_queued_requests - served_requests - discarded_requests, total_queued_requests, 100. * (total_queued_requests - served_requests - discarded_requests) / total_queued_requests);
	printf("|\n+-- Rejected requests (full queue): %u/%u (%.2lf%%)\n\n", rejected_requests, total_requests, 100. * rejected_requests / total_requests);
	
	double timebase_mult = 1./1000;
	double arrival_rate = total_requests / (max_simulation_time * timebase_mult);
	double throughput = served_requests / (max_simulation_time * timebase_mult);
	double reject_rate = rejected_requests / (max_simulation_time * timebase_mult);
	double timeout_rate = discarded_requests / (max_simulation_time * timebase_mult);
	double server_load = 0.;
	double server_usage = 0.;
	double queue_length_avg = 0.;
	uint32_t queue_length_max = 0;
	double queue_length_variance = 0.;
	double turnaround_avg = 0.;
	double turnaround_stdev = 0.;
	double service_avg = 0.;
	double service_stdev = 0.;
	
	uint32_t i;
	for (i = 0; i < request_log->q_length; i++)
	{
		http_request *req = (http_request*) queue_index(request_log, i);
		double turnaround = req->waiting_time + req->service_time;
		turnaround_avg += turnaround;
		service_avg += req->service_time;
		server_usage += req->service_time;
		//fprintf(stderr, "Request arrived at %lf: waited %lf, served in %lf\n", req->timestamp, req->waiting_time, req->service_time);
	}
	if (server_is_processing) server_usage += max_simulation_time - ((http_request*) queue_index(request_log, -1))->timestamp - ((http_request*) queue_index(request_log, -1))->waiting_time;
	turnaround_avg /= request_log->q_length;
	service_avg /= request_log->q_length;
	server_usage /= max_simulation_time;
	server_load = arrival_rate / (throughput / server_usage);
	for (i = 0; i < request_log->q_length; i++)
	{
		http_request *req = (http_request*) queue_index(request_log, i);
		double turnaround = req->waiting_time + req->service_time;
		double partial_stdev;
		partial_stdev = turnaround - turnaround_avg;
		turnaround_stdev += partial_stdev * partial_stdev;
		partial_stdev = req->service_time - service_avg;
		service_stdev += partial_stdev * partial_stdev;
	}
	turnaround_stdev /= request_log->q_length - 1;
	service_stdev /= request_log->q_length - 1;
	turnaround_stdev = sqrt(turnaround_stdev);
	service_stdev = sqrt(service_stdev);

	for (i = 0; i < request_queue_log->q_length; i++)
	{
		queue_len_log *dp = (queue_len_log*) queue_index(request_queue_log, i);
		queue_len_log *nextdp = (queue_len_log*) queue_index(request_queue_log, i + 1);
		queue_length_avg += dp->length * ((i + 1 < request_queue_log->q_length ? nextdp->timestamp : max_simulation_time) - dp->timestamp);
		if (dp->length > queue_length_max) queue_length_max = dp->length;
		//fprintf(stderr, "%lf: Queue log slice: %d * %lf = %lf -->\t%lf /\t%lf =\t%lf\n", dp->timestamp, dp->length, (i + 1 < request_queue_log->q_length ? nextdp->timestamp : max_simulation_time) - dp->timestamp, dp->length * ((i + 1 < request_queue_log->q_length ? nextdp->timestamp : max_simulation_time) - dp->timestamp), queue_length_avg, max_simulation_time, queue_length_avg / max_simulation_time);
	}
	queue_length_avg /= max_simulation_time;
	for (i = 0; i < request_queue_log->q_length; i++)
	{
		queue_len_log *dp = (queue_len_log*) queue_index(request_queue_log, i);
		queue_len_log *nextdp = (queue_len_log*) queue_index(request_queue_log, i + 1);
		double deviation = (dp->length - queue_length_avg);
		double partial_variance = deviation * deviation * ((i + 1 < request_queue_log->q_length ? nextdp->timestamp : max_simulation_time) - dp->timestamp) / max_simulation_time;
		queue_length_variance += partial_variance;
	}

	const char* unit = "requests/s";
	printf("Arrival rate: %lf %s\n", arrival_rate, unit);
	printf("Throughput: %lf %s\n", throughput, unit);
	printf("Avg dropped request rate: %lf %s\n", reject_rate, unit);
	printf("Avg discarded (timed out) request rate: %lf %s\n", timeout_rate, unit);
	printf("Server load: %lf\n", server_load);
	printf("Server busy time: %.3lf%%\n", server_usage * 100);
	printf("Calculated max throughput: %lf %s\n\n", throughput / server_usage, unit);
	printf("Queue length: avg %lf, max %u/%u, stdev %lf\n", queue_length_avg, queue_length_max, request_queue_length_max, sqrt(queue_length_variance));
	printf("Turnaround time: avg %lf, stdev %lf\n", turnaround_avg, turnaround_stdev);
	printf("Service time: avg %lf, stdev %lf\n", service_avg, service_stdev);

	request_queue = queue_free(request_queue);
	return 0;
}
