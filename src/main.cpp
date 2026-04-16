#include "./Board-Representation/Board.h"
#include "./Board-Representation/Move.h"
#include "./Movegeneration/Attacks.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace Thoth;

Board board;

int main() {
    Zobrist::initKeys();
    Attacks::init();

    board.parseFEN("8/8/8/8/8/8/3P4/2B5 w KQkq - 0 1");
    board.printBoard();

    board.printBitBoard(Attacks::getBishopAttacks(C1, board.getOccupancy(COLOR_NB)));


    return 0;
}