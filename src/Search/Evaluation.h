#pragma once
#include "../Board-Representation/Board.h"

namespace Thoth {
namespace Eval {

constexpr int pieceValues[PIECE_TYPE_NB] = {
    100,
    320,
    330,
    500,
    900,
    10000
};  


int evaluate(const Board board);


}
}