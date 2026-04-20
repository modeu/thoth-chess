#include "Search.h"
#include "Evaluation.h"

namespace Thoth {
namespace Search {


int Searcher::alphaBeta(Board &board, int depth, int alpha, int beta) {
    if (stop_->load(std::memory_order_relaxed))
        return 0;
    
    if (depth == 0)
        return Eval::evaluate(board);

    MoveList moves;
    Movegen::generateLegalMoves(board, moves);

    if (moves.size() == 0) {
        if (Movegen::isCheck(board, board.getSideToMove())) {
            return -100000 + board.getHistoryIndex();
        }
        return 0;
    }

    int bestScore = -99999;

    for (Move m : moves) {
        board.makeMove(m);
        int score = -alphaBeta(board, depth - 1, -beta, -alpha);
        board.unmakeMove();

        if (stop_->load(std::memory_order_relaxed))
            return 0;
        
        if (score > bestScore) {
            bestScore = score;
            if (score > alpha) {
                alpha = score;
                if (depth == currentDepth) {
                    bestMoveCurrentDepth = m;
                }
            }
        }

        if (score >= beta) {
            return bestScore;
        }
    }

    return bestScore;
}


}
}