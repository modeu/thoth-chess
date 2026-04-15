#pragma once
#include "../Board-Representation/Board.h"

namespace Thoth {
namespace Movegen {

void generateKnightMoves(const Board &board, MoveList &moves);
void generateKingMoves(const Board &board, MoveList &moves);
void generatePawnMoves(const Board &board, MoveList &moves);

template<PieceType PT>
void generateSlidingMoves(const Board &board, MoveList &moves);

void generatePseudoLegalMoves(const Board &board, MoveList &moves);
void generateLegalMoves(const Board &board, MoveList &moves);


}
}