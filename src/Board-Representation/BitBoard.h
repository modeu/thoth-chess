#pragma once
#include <bit>
#include "../Misc.h"

constexpr int countBits(BitBoard bb) {
    return __builtin_popcountll(bb);
}

constexpr int getLSB(BitBoard bb) {
    return __builtin_ctzll(bb);
}

inline int popLSB(BitBoard &bb) {
    int square = getLSB(bb);
    bb &= bb - 1;
    return square;
}