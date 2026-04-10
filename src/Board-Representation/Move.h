#pragma once
#include "../Misc.h"
#include <array>

namespace Thoth {
namespace Moves {
enum MoveFlag {
    NORMAL, DOUBLE_PAWN_PUSH, EN_PASSANT, CASTLE_KING, CASTLE_QUEEN, PROMOTION
};

constexpr Move NULL_MOVE = 0;

inline Move createMove(Square from, Square to, PieceType pt, MoveFlag flag = NORMAL, PieceType promoPt = QUEEN) {
    return (from) | (to << 6) | (pt << 12) | (flag << 16) | (promoPt << 19);
}

inline Square getFrom(Move m) {return static_cast<Square>(m & 0x3F);}
inline Square getTo(Move m) {return static_cast<Square>((m >> 6) & 0x3F);}
inline PieceType getPt(Move m) {return static_cast<PieceType>((m >> 12) & 0xF);}
inline MoveFlag getFlag(Move m) {return static_cast<MoveFlag>((m >> 16) & 0x7);}
inline PieceType getPromoPt (Move m) {return static_cast<PieceType>((m >> 19) & 0x7);}

inline bool isDoublePP(Move m) {return getFlag(m) == DOUBLE_PAWN_PUSH;}
inline bool isEnPassant(Move m) {return getFlag(m) == EN_PASSANT;}
inline bool isCastle(Move m) {return getFlag(m) == CASTLE_KING || getFlag(m) == CASTLE_QUEEN;}
inline bool isPromotion(Move m) {return getFlag(m) == PROMOTION;}
inline bool isNullMove(Move m) {return getFlag(m) == NULL_MOVE;}

}

struct MoveList {
    std::array<Move, 256> moves;
    int count = 0;

    void add(Move m) { moves[count++] = m; }
    void clear() { count = 0; }
    int size() const { return count; }
    
    Move* begin() { return moves.data(); }
    Move* end() { return moves.data() + count; }
};

}