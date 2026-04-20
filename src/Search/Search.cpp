#include "Search.h"
#include "Evaluation.h"
#include <chrono>
#include <iostream>

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


SearchResult Searcher::search(Board &board, int msTime, std::atomic<bool> &stop) {
    stop_ = &stop;

    SearchResult result;
    auto startTime = std::chrono::steady_clock::now();

    auto elapsed = [&]() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count();
    };

    for (int depth = 1; depth <= 100; depth++) {
        currentDepth = depth;
        long long beforeDepth = elapsed();

        int score = alphaBeta(board, depth, -99999, 99999);

        if (stop_->load(std::memory_order_relaxed))
            break;

        result.bestMove = bestMoveCurrentDepth;
        result.score = score;
        result.depth = depth;

        long long afterDepth = elapsed();
        long long depthTime = afterDepth - beforeDepth;

        int uciScore = (board.getSideToMove() == WHITE) ? score : -score;

        std::cout << "info depth " << depth
                  << " score cp " << uciScore
                  << " time " << depthTime
                  << "\n";
        std::cout.flush();

        constexpr int BRANCHING_FACTOR = 3;
        if (afterDepth + depthTime * BRANCHING_FACTOR > msTime) 
            break;
    }

    return result;
}


}
}