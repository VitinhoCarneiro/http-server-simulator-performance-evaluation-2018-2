#include "../random_var.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>

int main (int argc, char** argv)
{
	float mean = 4, rand_result, rand_mean = 0.f, rand_stdev = 0.f;
	int i, rounds = 10000;
       	uint32_t seed = 1;
	if (argc > 1)
	{
		if (!sscanf(argv[1], "%u", &seed))
		{
			printf("ERROR: Invalid value for seed `%s', defaulting to 1.\n\n", argv[1]);
		}
	}
	
	seed_rng(seed);
	for (i = 0; i < rounds; i++)
	{
		rand_result = rand_exponential(mean);
		rand_mean += rand_result;
		rand_stdev += (rand_result - mean) * (rand_result - mean);
		printf("%1.8f ", rand_result);
	}
	printf("\nMean = %f\n", rand_mean / rounds);
	printf("\nStd. dev = %f\n", sqrt(rand_stdev / (rounds - 1)));
	return 0;
}
