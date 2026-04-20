#pragma once
#include "../Movegeneration/Movegen.h"
#include <atomic>

namespace Thoth {
namespace Search {

struct SearchResult {
    Move bestMove = Moves::NULL_MOVE;
    int score = 0;
    int depth = 0;
};

class Searcher {
    private:
        int alphaBeta(Board &board, int depth, int alpha, int beta);

        Move bestMoveCurrentDepth = Moves::NULL_MOVE;
        Move bestMoveOverall = Moves::NULL_MOVE;
        int currentDepth = 0;

        std::atomic<bool> *stop_ = nullptr;

    public:
        SearchResult search(Board &board, int msTime);


};


}
}