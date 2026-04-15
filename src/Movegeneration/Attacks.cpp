#include "Attacks.h"
#include "../Board-Representation/BitBoard.h"

namespace Thoth {
namespace Attacks {

// Rook Magic Numbers
static constexpr BitBoard ROOK_MAGICS[64] = {
    0x18000844000f8a0ULL,
    0x2140100020024001ULL,
    0x680100184200008ULL,
    0x4080100004820800ULL,
    0x480040080020800ULL,
    0x480020080040001ULL,
    0x8400009c02090810ULL,
    0x80110002204980ULL,
    0xc2480018420c004ULL,
    0x1141002081104000ULL,
    0x2190808020001000ULL,
    0xa0801000080081ULL,
    0x800800800400ULL,
    0x2083000400081700ULL,
    0x40a5001402000100ULL,
    0x45000080620100ULL,
    0x4040008020800044ULL,
    0x820021004e00ULL,
    0x4c820040e21200ULL,
    0x2000808010000800ULL,
    0x400808004000800ULL,
    0x20a008004008002ULL,
    0xc628040001100208ULL,
    0x401020004aa4401ULL,
    0x810200220040ULL,
    0x4285044200220881ULL,
    0x838100080200481ULL,
    0x20120200082041ULL,
    0x1008004900110004ULL,
    0x140020080800400ULL,
    0x42100a50016000cULL,
    0x200010200308044ULL,
    0x80044002402000ULL,
    0x80e001804000ULL,
    0x104101002000ULL,
    0x4148100101000820ULL,
    0x6000802401800800ULL,
    0x8800200800400ULL,
    0x8c3000401000200ULL,
    0x8400450a001084ULL,
    0x804000218004ULL,
    0x20830080c0030020ULL,
    0x100020008080ULL,
    0x201c200208a0011ULL,
    0x202c040008008080ULL,
    0x42000824620010ULL,
    0x100020110040008ULL,
    0x40090520001ULL,
    0x1c142206c1048200ULL,
    0x20200418a250200ULL,
    0x400108020420200ULL,
    0x100080880280ULL,
    0x800c800400080080ULL,
    0x8004000200800480ULL,
    0x3911500918021400ULL,
    0x283c0189004600ULL,
    0x1001008010204202ULL,
    0x824410220102ULL,
    0x4011100901422001ULL,
    0x401100061000c29ULL,
    0x3000800100433ULL,
    0xb091000882040041ULL,
    0x30200008904281aULL,
    0x802004130280a242ULL,
};

// Bishop Magic Numbers
static constexpr BitBoard BISHOP_MAGICS[64] = {
    0x1102202c140a85a0ULL,
    0x2022080805004120ULL,
    0x49000a208400200ULL,
    0x24042289840880ULL,
    0x22021000801000ULL,
    0xa82015008810100ULL,
    0x404d4420201080ULL,
    0x10100700c1400ULL,
    0x4002208802081040ULL,
    0x45004010020ULL,
    0x40210342140cc00ULL,
    0x4104a008000ULL,
    0x10004242000440cULL,
    0x1002008804404000ULL,
    0x8407100d2104080ULL,
    0x2111090401212860ULL,
    0x8804001950100641ULL,
    0xa21005002021045ULL,
    0x214200410a020200ULL,
    0x848002082004050ULL,
    0x80060104021108c0ULL,
    0x200800110100101ULL,
    0x201020090882080ULL,
    0x894822108680200ULL,
    0x408200004041010ULL,
    0x10108c124080820ULL,
    0x2008010048220024ULL,
    0x404010000200880ULL,
    0x2084004c802028ULL,
    0xa0590002028a00ULL,
    0x10310110c2082112ULL,
    0x1008802101040208ULL,
    0x201002508049100cULL,
    0x1d08040400102140ULL,
    0x400402080100102ULL,
    0x2042020080080080ULL,
    0x1840088020120020ULL,
    0x101210100020040ULL,
    0x1a40101008808ULL,
    0x4070e10144040ULL,
    0x402080426404100ULL,
    0x141008820000406ULL,
    0x602008201002082ULL,
    0x200042008d1802ULL,
    0x20400091080600ULL,
    0x8040210420202101ULL,
    0x20010200991a00ULL,
    0x101040400440094ULL,
    0x10a022220044012ULL,
    0x5048404208210200ULL,
    0x1010382508088000ULL,
    0x2000000020880040ULL,
    0x10004214050c0100ULL,
    0x4103c008c2042800ULL,
    0x909900108111063ULL,
    0x20b8104100490004ULL,
    0x8280280504a000ULL,
    0x50c4184202012011ULL,
    0x13000600520800ULL,
    0x108c00000b421200ULL,
    0x82210800c0150440ULL,
    0x2110001010110842ULL,
    0x200880801480201ULL,
    0x5020600101070110ULL,
};

static BitBoard bishopAttacks[5248];
static BitBoard rookAttacks[102400];

BitBoard pawnAttacks[COLOR_NB][SQUARE_NB];
BitBoard knightAttacks[SQUARE_NB];
BitBoard kingAttacks[SQUARE_NB];

void initKnightTable() {
    for (int sq = 0; sq < SQUARE_NB; sq++) {
        BitBoard knight = squareBB((Square)sq);
        BitBoard attacks = 0ULL;

        if (knight & FILE_A_BB) {
            attacks |= knight >> 17;
            attacks |= knight << 15;
        }

        if (knight & FILE_AB_BB) {
            attacks |= knight >> 10;
            attacks |= knight << 6;
        } 

        if (knight & FILE_GH_BB) {
            attacks |= knight << 10;
            attacks |= knight >> 6;
        }
        
        if (knight & FILE_H_BB) {
            attacks |= knight << 17;
            attacks |= knight >> 15;
        }

        knightAttacks[sq] = attacks;
    }
}

void initKingTable() {
    for (int sq = 0; sq < SQUARE_NB; sq++) {
        BitBoard king = squareBB((Square)sq);
        BitBoard attacks = 0ULL;

        attacks |= (king << 8) | (king >> 8);
        attacks |= (king & FILE_H_BB) << 1 | (king & FILE_A_BB) >> 1;
        attacks |= (king & FILE_H_BB) << 9 | (king & FILE_A_BB) << 7;
        attacks |= (king & FILE_H_BB) >> 7 | (king & FILE_A_BB) >> 9; 

        kingAttacks[sq] = attacks;
    }
}

void initPawnTable() {
    for (int sq = 0; sq < SQUARE_NB; sq++) {
        BitBoard pawn = squareBB((Square)sq);
        BitBoard whiteAttacks = 0ULL;
        BitBoard blackAttacks = 0ULL;

        whiteAttacks |= ((pawn & FILE_H_BB) << 9) | ((pawn & FILE_A_BB) << 7);
        blackAttacks |= ((pawn & FILE_H_BB) >> 7) | ((pawn & FILE_A_BB) >> 9);

        pawnAttacks[WHITE][sq] = whiteAttacks;
        pawnAttacks[BLACK][sq] = blackAttacks;
    }
}

void initMagics() {
    BitBoard *rookPtr = rookAttacks;
    BitBoard *bishopPtr = bishopAttacks;

    for (int sq = 0; sq < SQUARE_NB; sq++) {
        rookMagics[sq].attacks = rookPtr;
        bishopMagics[sq].attacks = bishopPtr;

        int rookBits = countBits(rookMagics[sq].mask);
        int bishopBits = countBits(bishopMagics[sq].mask);

        rookPtr += (1 << rookBits);
        bishopPtr += (1 << bishopBits);
    }
}    


void init() {
    initKnightTable();
    initKingTable();
    initPawnTable();
}





}
}
