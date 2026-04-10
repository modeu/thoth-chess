#include "Board-Representation/Board.h"
#include "Board-Representation/Move.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace Thoth;
using namespace Moves;

Board board;

int main() {
    MoveList list;
    Move m = createMove(E7, E8, PAWN, PROMOTION, ROOK);
    Move m1 = createMove(E7, E8, PAWN, PROMOTION, ROOK);
    Move m2 = createMove(A1, A2, ROOK);
    
    std::cout << "From: " << +getFrom(m) << std::endl;
    std::cout << "To: " << +getTo(m) << std::endl;
    std::cout << "Piece Type: " << +getPt(m) << std::endl;
    std::cout << "Flags: " << "IsNormal: " << isNormalMove(m) << " | "  << "IsDoublePP: " << isDoublePP(m) << " | " << "IsEnPassant: " << isEnPassant(m) << " | " << "IsCastle: " << isCastle(m) << " | " << "IsPromotion: " << isPromotion(m) << std::endl;
    std::cout << "Promo: " << +getPromoPt(m) << std::endl;

    list.add(m);
    list.add(m1);
    list.add(m2);

    std::cout << "List Size: " << list.size() << std::endl;
    list.clear();
    std::cout << "List Size: " << list.size() << std::endl;

    return 0;
}