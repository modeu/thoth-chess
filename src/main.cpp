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

    board.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    board.printBitBoard(Attacks::getRookAttacks(A1, board.getOccupancy(COLOR_NB)));


    return 0;
}