#include "ZobristKeys.h"
#include <random>

uint64_t pieceKeys[2][6][64];
uint64_t enPassantKeys[8];
uint64_t castlingKeys[16];
uint64_t sideKey;

void initZobristKeys() {
    std::mt19937_64 rng(0); //Fixed seed for reproducibility
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);

    for (int color = 0; color < 2; color++) {
        for (int pieceType = 0; pieceType < 6; pieceType++) {
            for (int square = 0; square < 64; square++) {
                pieceKeys[color][pieceType][square] = dist(rng);
            }
        }
    }

    for (int square = 0; square < 64; square++) {
        enPassantKeys[square] = dist(rng);
    }

    for (int i = 0; i < 16; i++) {
        castlingKeys[i] = dist(rng);
    }

    sideKey = dist(rng);
}