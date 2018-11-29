#include "random_var.h"
#include <math.h>

static rng32_state rng = 1;

void seed_rng (rng32_state seed)
{
	rng = seed;
}

static inline float _rand_uniform ()
{
	return (((float) rng_xorshift(&rng)) / (float) 4294967295);
}

float rand_exponential (float mean)
{
	return -mean * log(1 - _rand_uniform());
}
