#include "Movegen.h"
#include "Attacks.h"

namespace Thoth {
namespace Movegen {

void generateKnightMoves(const Board &board, MoveList &moves) {
    Color side = board.getSideToMove();
    BitBoard knights = board.getPiece(side, KNIGHT);

    while (knights) {
        Square from = (Square)popLSB(knights);
        BitBoard attacks = Attacks::knightAttacks[from] & ~board.getOccupancy(side);

        while(attacks) {
            Square to = (Square)popLSB(attacks);
            moves.add(Moves::createMove(from, to , KNIGHT));
        }
    }
}

bool isSquareAttacked(const Board &board, Square square, Color attacker) {
    BitBoard occ = board.getOccupancy(COLOR_NB);
    BitBoard bishopQueenAttacks = Attacks::getBishopAttacks(square, occ);
    BitBoard rookQueenAttacks = Attacks::getRookAttacks(square, occ);

    if (bishopQueenAttacks & (board.getPiece(attacker, BISHOP) | board.getPiece(attacker, QUEEN))) return true;
    if (rookQueenAttacks & (board.getPiece(attacker, ROOK) | board.getPiece(attacker, QUEEN))) return true;
    if (Attacks::knightAttacks[square] & board.getPiece(attacker, KNIGHT)) return true;
    if (Attacks::pawnAttacks[attacker][square] & board.getPiece(attacker, PAWN)) return true;
    if (Attacks::kingAttacks[square] & board.getPiece(attacker, KING)) return true;

    return false;
}

bool isCheck(const Board &board, Color side) {
    Square kingSquare = (Square)getLSB(board.getPiece(side, KING));
    return isSquareAttacked(board, kingSquare, ~side);
}

void generateKingMoves(const Board &board, MoveList &moves) {
    Color side = board.getSideToMove();
    BitBoard king = board.getPiece(side, KING);
    BitBoard ownPieces = board.getOccupancy(side);
    BitBoard allPieces = board.getOccupancy(COLOR_NB);

    Square from = (Square)getLSB(king);
    BitBoard attacks = Attacks::kingAttacks[from] & ~ownPieces;

    while(attacks) {
        Square to = (Square)popLSB(attacks);
        moves.add(Moves::createMove(from, to, KING));
    }

    

    if (side == WHITE) {
        bool kingNotInCheck = !isCheck(board, WHITE);

        if ((board.getCastlingRights() & CASTLE_WK) && 
            !(allPieces & (squareBB(F1) | squareBB(G1))) && 
            !isSquareAttacked(board, F1, BLACK) &&
            kingNotInCheck) {
            moves.add(Moves::createMove(E1, G1, KING, Moves::CASTLE_KING));
        }

        if ((board.getCastlingRights() & CASTLE_WQ) && 
            !(allPieces & (squareBB(D1) | squareBB(C1) | squareBB(B1))) && 
            !isSquareAttacked(board, D1, BLACK) &&
            kingNotInCheck) {
            moves.add(Moves::createMove(E1, C1, KING, Moves::CASTLE_QUEEN));
        }
    } else {
        bool kingNotInCheck = !isCheck(board, BLACK);

        if ((board.getCastlingRights() & CASTLE_BK) && 
            !(allPieces & (squareBB(F8) | squareBB(G8))) && 
            !isSquareAttacked(board, F8, WHITE) &&
            kingNotInCheck) {
            moves.add(Moves::createMove(E8, G8, KING, Moves::CASTLE_KING));
        }

        if ((board.getCastlingRights() & CASTLE_BQ) && 
            !(allPieces & (squareBB(D8) | squareBB(C8) | squareBB(B8))) && 
            !isSquareAttacked(board, D8, WHITE) &&
            kingNotInCheck) {
            moves.add(Moves::createMove(E8, C8, KING, Moves::CASTLE_QUEEN));
        }
    }
}

void generatePawnMoves(const Board &board, MoveList &moves) {
    Color side = board.getSideToMove();
    BitBoard pawns = board.getPiece(side, PAWN);
    BitBoard enemyPieces = board.getOccupancy(~side);
    BitBoard emptySquares = ~board.getOccupancy(COLOR_NB);

    int direction = (side == WHITE) ? 8 : -8;
    BitBoard promotionRank = (side == WHITE) ? RANK_7_BB : RANK_2_BB;
    BitBoard startingRank = (side == WHITE) ? RANK_2_BB : RANK_7_BB;

    BitBoard enPassantSq = (board.getEnPassantSquare() != NO_SQUARE) ? squareBB(board.getEnPassantSquare()) : 0ULL;

    while(pawns) {
        Square from = (Square)popLSB(pawns);
        int rank = rankOf(from);

        int to = from + direction;
        if (to >= A1 && to <= H8) {
            int nextRank = rankOf((Square)to);
            if (emptySquares & squareBB((Square)to)) {
                
            }
        }
    }
}


}
}