#pragma once
#include <cstdint>

extern uint64_t pieceKeys[2][6][64]; //Zobrist keys for pieces (color, piece type, square)
extern uint64_t enPassantKeys[8]; //Zobrist keys for en passant squares
extern uint64_t castlingKeys[16]; //Zobrist keys for castling rights (4 bits for each side)
extern uint64_t sideKey; //Zobrist key for the side to move

void initZobristKeys(); //Function to initialize the Zobrist keys with random values
