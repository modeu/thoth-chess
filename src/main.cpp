#include "./Board-Representation/Board.h"
#include "./Board-Representation/Move.h"
#include "./Movegeneration/Attacks.h"
#include "./Movegeneration/Movegen.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace Thoth;

Board board;

uint64_t perft(int depth) {
    if (depth == 0) return 1ULL;

    MoveList moves;
    uint64_t nodes = 0;
    Movegen::generateLegalMoves(board, moves);

    for (Move m : moves) {
        board.makeMove(m);
        nodes += perft(depth - 1);
        board.unmakeMove();
    }
    return nodes;
}

void perftDivide(int depth) {
    MoveList moves;
    uint64_t total = 0;
    Movegen::generateLegalMoves(board, moves);
    
    for (Move m : moves) {
        // Move in UCI Format ausgeben
        Square from = Moves::getFrom(m);
        Square to   = Moves::getTo(m);
        
        // Square zu String
        auto squareStr = [](Square sq) -> std::string {
            std::string s;
            s += (char)('a' + fileOf(sq));
            s += (char)('1' + rankOf(sq));
            return s;
        };
        
        std::string moveStr = squareStr(from) + squareStr(to);
        
        // Promotion anhängen
        if (Moves::isPromotion(m)) {
            switch (Moves::getPromoPt(m)) {
                case QUEEN:  moveStr += 'q'; break;
                case ROOK:   moveStr += 'r'; break;
                case BISHOP: moveStr += 'b'; break;
                case KNIGHT: moveStr += 'n'; break;
                default: break;
            }
        }
        
        board.makeMove(m);
        uint64_t nodes = perft(depth - 1);
        board.unmakeMove();
        
        std::cout << moveStr << ": " << nodes << "\n";
        total += nodes;
    }
    
    std::cout << "\nNodes searched: " << total << "\n";
}

int main() {
    Zobrist::initKeys();
    Attacks::init();

    board.parseFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");

    perftDivide(4);
    return 0;
}