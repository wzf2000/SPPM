#include <cstdlib>
#include <cassert>

class Sampler {
	float rndOffset;
	unsigned currPrime;
	int currPass;
public:
    Sampler(int pass, uint32_t seed);
    float get();
};
