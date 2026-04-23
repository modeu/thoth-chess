#pragma once
#include "../Movegeneration/Movegen.h"
#include <atomic>

namespace Thoth {
namespace Search {

constexpr int MVV_LVA[PIECE_TYPE_NB][PIECE_TYPE_NB] = {
    //          P    N    B    R    Q    K
/* P */  { 105, 205, 305, 405, 505, 605 },
/* N */  { 104, 204, 304, 404, 504, 604 },
/* B */  { 103, 203, 303, 403, 503, 603 },
/* R */  { 102, 202, 302, 402, 502, 602 },
/* Q */  { 101, 201, 301, 401, 501, 601 },
/* K */  { 100, 200, 300, 400, 500, 600 }
};

struct SearchResult {
    Move bestMove = Moves::NULL_MOVE;
    int score = 0;
    int depth = 0;
};

class Searcher {
    private:
        int alphaBeta(Board &board, int depth, int alpha, int beta);
        int scoreMove(const Board &board, Move move);
        void sortMoves(const Board &board, MoveList &moves);

        Move bestMoveCurrentDepth = Moves::NULL_MOVE;
        Move bestMoveOverall = Moves::NULL_MOVE;
        int currentDepth = 0;

        std::atomic<bool> *stop_ = nullptr;

    public:
        SearchResult search(Board &board, int msTime, std::atomic<bool> &stop);


};


}
}