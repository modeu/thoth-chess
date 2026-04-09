#pragma once
#include <bit>
#include "../Misc.h"

constexpr int countBits(BitBoard bb) {
    return std::__popcount(bb);
}

constexpr int getLSB(BitBoard bb) {
    return std::__countr_zero(bb);
}

constexpr int getMSB(BitBoard bb) {
    return 63 - std::__countl_zero(bb);
}

inline int popLSB(BitBoard &bb) {
    int square = getLSB(bb);
    bb &= bb - 1;
    return square;
}

