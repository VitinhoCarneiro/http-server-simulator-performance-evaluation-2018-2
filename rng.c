#include "rng.h"

#ifndef STDINT
#define STDINT
#include <stdint.h>
#endif

/* ----------------------------------------------------------------------------
 * Implements the Xorshift RNG algorithm by George Marsaglia.
 * The state must be initialized to a nonzero value. 
 * ------------------------------------------------------------------------- */
uint32_t rng_xorshift (_rng32_state *rng)
{	
	*rng ^= *rng << 13;
	*rng ^= *rng >> 17;
	*rng ^= *rng << 5;
	return *rng;
}
