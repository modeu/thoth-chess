#include "./Movegeneration/Attacks.h"
#include "./UCI/Uci.h"

using namespace Thoth;

UCI uci;

int main() {
    Zobrist::initKeys();
    Attacks::init();

    uci.loop();

   
    return 0;
}