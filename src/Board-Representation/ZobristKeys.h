#pragma once
#include "../Misc.h"

namespace Zobrist {
extern uint64_t pieceKeys[COLOR_NB][PIECE_TYPE_NB][SQUARE_NB]; //Zobrist keys for pieces (color, piece type, square)
extern uint64_t sideKey; //Zobrist key for the side to move
extern uint64_t enPassantKeys[8]; //Zobrist keys for en passant squares
extern uint64_t castlingKeys[16]; //Zobrist keys for castling rights (4 bits for each side)

uint64_t prng(uint64_t &seed);
void initKeys(); //Function to initialize the Zobrist keys with random values
}