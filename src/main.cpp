#include "Board-Representation/Board.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace Thoth;

Board board;

int main() {
    Zobrist::initKeys();
    board.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    board.printBoard();

    std::cout << std::endl << "ZobristHash: " << board.getZobristHash() << std::endl;
    

    return 0;
}