#include "ZobristKeys.h"

namespace Thoth {
namespace Zobrist {

uint64_t pieceKeys[COLOR_NB][PIECE_TYPE_NB][SQUARE_NB];
uint64_t sideKey;
uint64_t enPassantKeys[8];
uint64_t castlingKeys[16];

uint64_t prng(uint64_t &seed) {
    uint64_t z = (seed += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}

void initKeys() {
    uint64_t seed = 0x9E3779B97F4A7C15ULL;
    for (int color = 0; color < 2; color++) {
        for (int pieceType = 0; pieceType < 6; pieceType++) {
            for (int square = 0; square < 64; square++) {
                pieceKeys[color][pieceType][square] = prng(seed);
            }
        }
    }

    for (int square = 0; square < 8; square++) {
        enPassantKeys[square] = prng(seed);
    }

    for (int i = 0; i < 16; i++) {
        castlingKeys[i] = prng(seed);
    }

    sideKey = prng(seed);
}
}
}