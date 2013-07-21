/*
Copyright (c) 2013 Benedikt Bitterli

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

   3. This notice may not be removed or altered from any source
   distribution.
*/

#include <stdint.h>
#include <algorithm>

#include "Util.hpp"

float frand() {
    static uint32_t seed = 0xBA5EBA11;
    seed = (seed*1103515245 + 12345) & 0x7FFFFFFF;

    union { uint32_t i; float f; } union_hack;

    union_hack.i = (seed >> 8) | 0x3F800000;
    return union_hack.f - 1.0;
}

int irand() {
    static uint32_t seed = 0xDEADBEEF;
    seed = (seed*1103515245 + 12345) & 0x7FFFFFFF;

    return seed;
}

float smoothStep(float edge0, float edge1, float x) {
    x = std::min(std::max((x - edge0)/(edge1 - edge0), 0.0f), 1.0f);

    return x*x*(3.0f - 2.0f*x);
}

int stringHash(const char *str) {
    int hash = 5381;

    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}
