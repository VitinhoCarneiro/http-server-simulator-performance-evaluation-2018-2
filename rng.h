#ifndef STDINT
#define STDINT
#include <stdint.h>
#endif

typedef uint32_t rng32_state;

/* ----------------------------------------------------------------------------
 * Cycles through one iteration of the 32-bit RNG using the Xorshift algorithm
 * by George Marsaglia.
 * OBS: The state must be initialized to a nonzero value.
 * ------------------------------------------------------------------------- */
uint32_t rng_xorshift (rng32_state *rng);
