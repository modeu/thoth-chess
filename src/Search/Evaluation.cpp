#include "Evaluation.h"

namespace Thoth {
namespace Eval {


int evaluate(const Board board) {
    int score = 0;

    for (int pt = PAWN; pt < PIECE_TYPE_NB; pt++) {
        score += countBits(board.getPiece(WHITE, (PieceType)pt)) * pieceValues[pt];
        score -= countBits(board.getPiece(BLACK, (PieceType)pt)) * pieceValues[pt];
    }

    return board.getSideToMove() == WHITE ? score : -score;
}


}
}