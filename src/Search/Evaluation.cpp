#include "Evaluation.h"

namespace Thoth {
namespace Eval {


int calcPhase(const Board board) {
    int phase = TOTAL_PHASE;
    for (int pt = PAWN; pt < PIECE_TYPE_NB; pt++) {
        phase -= countBits(board.getPiece(WHITE, (PieceType)pt)) * phaseWeights[pt];
        phase -= countBits(board.getPiece(BLACK, (PieceType)pt)) * phaseWeights[pt];
    }

    return (phase * 256 + TOTAL_PHASE/2) / TOTAL_PHASE;
}

constexpr int pstIndex(int sq, Color c) {
    return (c == WHITE) ? (sq ^ 56) : sq;
}

int evaluate(const Board board) {
    int score = 0;

    for (int pt = PAWN; pt < PIECE_TYPE_NB; pt++) {
        score += countBits(board.getPiece(WHITE, (PieceType)pt)) * pieceValues[pt];
        score -= countBits(board.getPiece(BLACK, (PieceType)pt)) * pieceValues[pt];
    }

    for (int sq = 0; sq < SQUARE_NB; sq++) {
        int tableScore = 0;
        Piece p = board.getPieceOn((Square)sq);
        if (p == NO_PIECE) continue;

        int idx = pstIndex(sq, colorOf(p));

        switch(typeOf(p)) {
            case PAWN:
                tableScore = pawnPieceTable[idx];
                break;
            case KNIGHT:
                tableScore = knightPieceTable[idx];
                break;
            case BISHOP:
                tableScore = bishopPieceTable[idx];
                break;
            case ROOK:
                tableScore = rookPieceTable[idx];
                break;
            case QUEEN:
                tableScore = queenPieceTable[idx];
                break;
            case KING:
                int gamePhase = calcPhase(board);
                tableScore = (kingPieceTableMid[idx] * (256 - gamePhase) + kingPieceTableEnd[idx] * gamePhase) / 256;
                break;
        }

        score = (colorOf(p) == WHITE) ? score + tableScore : score - tableScore;
    }



    return board.getSideToMove() == WHITE ? score : -score;
}


}
}