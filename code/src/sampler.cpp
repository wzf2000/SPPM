#include "sampler.hpp"
#include "math.hpp"

static const unsigned int primes[] = {2, 3, 5, 7, 11, 13};
static const unsigned int primesCount = 6;

Sampler::Sampler(int pass, uint32_t seed) {
    rndOffset = uintToUnitReal(seed);
    currPass = pass;
    currPrime = 0;
}

float Sampler::get() {
    float v = decimalAdd(radicalInverse(currPass, primes[currPrime++ % primesCount]), rndOffset);
    assert(v >= 0);
    assert(v < 1);
    return v;
}
