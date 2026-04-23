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


int Searcher::scoreMove(const Board &board, Move move) {
    PieceType pt = Moves::getPt(move);
    Piece captured = board.getPieceOn(Moves::getTo(move));

    if (Moves::isEnPassant(move)) return MVV_LVA[PAWN][PAWN];

    if (captured == NO_PIECE) return 0;

    return MVV_LVA[pt][captured];
}

void Searcher::sortMoves(const Board &board, MoveList &moves) {
    std::array<int, 256> scores;
    for (int i = 0; i < moves.size(); i++)
        scores[i] = scoreMove(board, moves.moves[i]);

    for (int i = 1; i < moves.size(); i++) {
        Move keyMove = moves.moves[i];
        int keyScore = scores[i];
        int j = i - 1;

        while (j >= 0 && scores[j] < keyScore) {
            moves.moves[j + 1] = moves.moves[j];
            scores[j + 1] = scores[j];
            j--;
        }

        moves.moves[j + 1] = keyMove;
        scores[j + 1] = keyScore;
    }
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