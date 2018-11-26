#include "../rng.h"

#ifndef STDINT
#define STDINT
#include <stdint.h>
#endif

#ifndef STDIO
#define STDIO
#include <stdio.h>
#endif

int main ()
{
	int i;
	_rng32_state rng[1];

	*rng = 1;
	for (i = 0; i < 30; i++)
	{
		printf("%u\n", rng_xorshift(rng));
	}
}
