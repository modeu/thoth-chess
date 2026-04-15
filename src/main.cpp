#include "Board-Representation/Board.h"
#include "Board-Representation/Move.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace Thoth;
using namespace Moves;

Board board;

int main() {
    Zobrist::initKeys();
    board.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    Move move = createMove(A1, A5, ROOK);

    board.printBoard();
    board.makeMove(move);
    board.unmakeMove();
    board.printBoard();

    std::cout << +board.getCastlingRights() << std::endl;


    return 0;
}