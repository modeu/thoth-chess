#include "Movegen.h"
#include "Attacks.h"
#include <cassert>

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
    if (Attacks::pawnAttacks[~attacker][square] & board.getPiece(attacker, PAWN)) return true;
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
    Square enPassantSquare = board.getEnPassantSquare();

    int direction = (side == WHITE) ? 8 : -8;
    int promotionRank = (side == WHITE) ? 7 : 0;
    int startingRank = (side == WHITE) ? 1 : 6;

    auto addPromotions = [&](Square from, Square to) {
        moves.add(Moves::createMove(from, to, PAWN, Moves::PROMOTION, QUEEN));
        moves.add(Moves::createMove(from, to, PAWN, Moves::PROMOTION, ROOK));
        moves.add(Moves::createMove(from, to, PAWN, Moves::PROMOTION, BISHOP));
        moves.add(Moves::createMove(from, to, PAWN, Moves::PROMOTION, KNIGHT));
    };

    while(pawns) {
        Square from = (Square)popLSB(pawns);

        //Pushes
        Square to = from + direction;
        if (emptySquares & squareBB(to)) {
            if (rankOf(to) == promotionRank) {
                addPromotions(from, to);
            } else {
                moves.add(Moves::createMove(from, to, PAWN));
            }

            //Double
            if (rankOf(from) == startingRank) {
                Square doublePushTo = from + 2*direction;
                if (emptySquares & squareBB(doublePushTo)) {
                    moves.add(Moves::createMove(from, doublePushTo, PAWN, Moves::DOUBLE_PAWN_PUSH));
                }
            }
        }

        //Captures
        BitBoard enPassantBB = (enPassantSquare != NO_SQUARE) ? squareBB(enPassantSquare) : 0ULL;
        BitBoard attacks = Attacks::pawnAttacks[side][from] & (enemyPieces | enPassantBB);
        while (attacks) {
            Square captureTo = (Square)popLSB(attacks);

            if (captureTo == enPassantSquare) {
                moves.add(Moves::createMove(from, captureTo, PAWN, Moves::EN_PASSANT));
                continue;
            }

            if (rankOf(captureTo) == promotionRank) {
                addPromotions(from, captureTo);
            } else {
                moves.add(Moves::createMove(from, captureTo, PAWN));
            }
        }
    }
}

template<PieceType PT>
void generateSlidingMoves(const Board &board, MoveList &moves) {
    Color side = board.getSideToMove();
    BitBoard pieces = board.getPiece(side, PT);
    BitBoard ownPieces = board.getOccupancy(side);
    BitBoard allPieces = board.getOccupancy(COLOR_NB);

    while(pieces) {
        Square from = (Square)popLSB(pieces);
        BitBoard attacks = 0ULL;

        if constexpr (PT == BISHOP) {
            attacks |= Attacks::getBishopAttacks(from, allPieces) & ~ownPieces;
        }

        if constexpr (PT == ROOK) {
            attacks |= Attacks::getRookAttacks(from, allPieces) & ~ownPieces;
        }

        if constexpr (PT == QUEEN) {
            attacks |= Attacks::getQueenAttacks(from, allPieces) & ~ownPieces;
        }

        while(attacks) {
            Square to = (Square)popLSB(attacks);
            moves.add(Moves::createMove(from, to, PT));
        }
    }
}

void generatePseudoLegalMoves(const Board &board, MoveList &moves) {
    generatePawnMoves(board, moves);
    generateKnightMoves(board, moves);
    generateKingMoves(board, moves);
    generateSlidingMoves<BISHOP>(board, moves);
    generateSlidingMoves<ROOK>(board, moves);
    generateSlidingMoves<QUEEN>(board, moves);
}

void generateLegalMoves(Board &board, MoveList &moves) {
    MoveList pseudoMoves;
    generatePseudoLegalMoves(board, pseudoMoves);

    for (Move m : pseudoMoves) {
        Square from    = Moves::getFrom(m);
        Square to      = Moves::getTo(m);
        PieceType pt   = Moves::getPt(m);

        // Debug
        assert(pt >= PAWN && pt <= KING);
        assert(from >= A1 && from <= H8);
        assert(to   >= A1 && to   <= H8);


        board.makeMove(m);

        if (!isCheck(board, ~board.getSideToMove())) moves.add(m);
        
        board.unmakeMove();
    }
}

}
}