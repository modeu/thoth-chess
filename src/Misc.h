#pragma once
#include <cstdint>

namespace Thoth {

using BitBoard = uint64_t;
using Piece = int8_t;
using Move = uint32_t;

constexpr Piece NO_PIECE = -1;

constexpr BitBoard FILE_A_BB = 0xfefefefefefefefeULL;
constexpr BitBoard FILE_AB_BB = 0xfcfcfcfcfcfcfcfcULL;
constexpr BitBoard FILE_GH_BB = 0x3f3f3f3f3f3f3f3fULL;
constexpr BitBoard FILE_H_BB = 0x7f7f7f7f7f7f7f7fULL;

constexpr BitBoard RANK_2_BB = 0x000000000000ff00ULL;
constexpr BitBoard RANK_7_BB = 0x00ff000000000000ULL;

enum Square : uint8_t {
    A1, B1, C1, D1, E1, F1, G1, H1, A2, B2, C2, D2, E2, F2, G2, H2, A3, B3, C3, D3, E3, F3, G3, H3, A4, B4, C4, D4, E4, F4, G4, H4, A5, B5, C5, D5, E5, F5, G5, H5, A6, B6, C6, D6, E6, F6, G6, H6, A7, B7, C7, D7, E7, F7, G7, H7, A8, B8, C8, D8, E8, F8, G8, H8, 
    SQUARE_NB, NO_SQUARE = 255
};

enum PieceType : uint8_t {
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, 
    PIECE_TYPE_NB
};

enum Color : uint8_t {
    WHITE, BLACK,
    COLOR_NB
};

constexpr Color operator~(Color c) {
    return static_cast<Color>(c ^ 1);
}

constexpr int fileOf(Square sq) {return sq % 8;}
constexpr int rankOf(Square sq) {return sq / 8;}

constexpr Square makeSquare(int file, int rank) {
    return static_cast<Square>(rank * 8 + file);
}

constexpr Piece makePiece(Color c, PieceType pt) {
    return static_cast<Piece>((int)c * (int)PIECE_TYPE_NB + (int)pt);
}

constexpr Color colorOf(Piece p) {
    return static_cast<Color>(p / PIECE_TYPE_NB);
}

constexpr PieceType typeOf(Piece p) {
    return static_cast<PieceType>(p % PIECE_TYPE_NB);
}

constexpr BitBoard squareBB(Square sq) {
    return (1ULL << sq);
}


}