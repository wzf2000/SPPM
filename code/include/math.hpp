#include <iostream>
#include <random>

namespace Math {

    const double alpha = 0.7;

    static double random01() {
        static std::mt19937 *generator = nullptr;
        if (!generator) 
            generator = new std::mt19937(clock());
        static std::uniform_real_distribution<> dis(0, 1);
        return dis(*generator);
    }

    double random(double l, double r) {
        return l + random01() * (r - l);
    }

    static inline uint32_t hash(uint32_t x) {
        x  = (x ^ 12345391) * 2654435769;
        x ^= (x << 6) ^ (x >> 26);
        x *= 2654435769;
        x += (x << 5) ^ (x >> 12);
        return x;
    }

    static inline uint64_t hash64(uint64_t x) {
        x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
        x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
        x = x ^ (x >> 31);
        return x;
    }

}
