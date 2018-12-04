#include "random_var.h"
#include <math.h>

static rng32_state rng = 1;

void seed_rng (rng32_state seed)
{
	rng = seed;
}

static inline float _rand_uniform ()
{
	/* ------------------------------------------------------------------
	 * Multiplying by 0.999999 avoids this function from returning 1.0, |
	 * which would cause issues with the function below returning       |
	 * infinity.                                                        |
	 * ----------------------------------------------------------------*/
	return (((float) rng_xorshift(&rng)) / (float) 4294967296) * 0.999999f;
}

float rand_exponential (float mean)
{
	return -mean * log(1 - _rand_uniform());
}
