#include "Board-Representation/Board.h"
#include <iostream>

using namespace Thoth;

Board board;

int main() {
    board.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    board.printBoard();

    return 0;
}