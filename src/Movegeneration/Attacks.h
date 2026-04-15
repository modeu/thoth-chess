#pragma once
#include "../Misc.h"

namespace Thoth {
namespace Attacks {

extern BitBoard pawnAttacks[COLOR_NB][SQUARE_NB];
extern BitBoard knightAttacks[SQUARE_NB];
extern BitBoard kingAttacks[SQUARE_NB];

struct Magic {
    BitBoard mask;
    BitBoard magic;
    BitBoard *attacks;
    int shift;
};

extern Magic bishopMagics[SQUARE_NB];
extern Magic rookMagics[SQUARE_NB];

inline BitBoard getBishopAttacks(Square sq, BitBoard occupancy) {
    const Magic &m = bishopMagics[sq];
    return m.attacks[((occupancy & m.mask) * m.magic) >> m.shift];
}
    
inline BitBoard getRookAttacks(Square sq, BitBoard occupancy) {
    const Magic &m = rookMagics[sq];
    return m.attacks[((occupancy & m.mask) * m.magic) >> m.shift];
}

inline BitBoard getQueenAttacks(Square sq, BitBoard occupancy) {
    return getBishopAttacks(sq, occupancy) | getRookAttacks(sq, occupancy);
}

void initKnightTable();
void initKingTable();   
void initPawnTable();
void initMagics();
void init();


}
}