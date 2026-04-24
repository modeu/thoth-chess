#include "Uci.h"
#include <iostream>
#include <sstream>

namespace Thoth {

UCI::UCI() {

}

void UCI::loop() {
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        auto tokens = split(line);
        const auto &cmd = tokens[0];

        if (cmd == "uci") handleUci();
        else if (cmd == "isready") handleIsReady();
        else if (cmd == "ucinewgame") handleUciNewGame();
        else if (cmd == "position") handlePosition(tokens);
        else if (cmd == "go") handleGo(tokens);
        else if (cmd == "stop") handleStop();
        else if (cmd == "quit") break;
    }

    handleStop();
}

void UCI::handleUci() {
    send("id name Thoth");
    send("id author Moritz");
    send("uciok");
}

void UCI::handleIsReady() {
    send("readyok");
}

void UCI::handleUciNewGame() {
    handleStop();
    board_.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void UCI::handlePosition(const std::vector<std::string> &tokens) {
    int movesIdx = -1;

    if (tokens.size() < 2) return;

    if (tokens[1] == "startpos") {
        board_.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        movesIdx = 2;
    } else if (tokens[1] == "fen" && tokens.size() >= 8) {
        std::string fen =  tokens[2] + " " + tokens[3] + " " + tokens[4] + " " + tokens[5] + " " + tokens[6] + " " + tokens[7];
        board_.parseFEN(fen);
        movesIdx = 8;
    } 
    
    if (movesIdx != -1 && movesIdx < tokens.size() && tokens[movesIdx] == "moves") {
        for (int i = movesIdx + 1; i < tokens.size(); i++) {
            Move m = stringToMove(tokens[i]);
            if (m != Moves::NULL_MOVE) {
                board_.makeMove(m);
            }
        }
    }
}

void UCI::handleGo(const std::vector<std::string> &tokens) {
    handleStop();
    stop_.store(false);

    int msTime = 5000;
    Color side = board_.getSideToMove();

    int wtime = 0, btime = 0, winc = 0, binc = 0, moveTime = 0;

    for (int i = 1; i < (int)tokens.size(); i++) {
        if (tokens[i] == "wtime") wtime = std::stoi(tokens[i+1]);
        if (tokens[i] == "btime") btime = std::stoi(tokens[i+1]);
        if (tokens[i] == "winc") winc = std::stoi(tokens[i+1]);
        if (tokens[i] == "binc") binc = std::stoi(tokens[i+1]);
        if (tokens[i] == "movetime") moveTime = std::stoi(tokens[i+1]);
    }

    if (moveTime > 0) {
        msTime = moveTime;
    } else if (side == WHITE) {
        msTime = wtime/20 + winc/2;
    } else if (side == BLACK) {
        msTime = btime/20 + binc/2;
    }



    searchThread_ = std::thread([this, msTime]() {
        std::thread timer([this, msTime]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(msTime));
            stop_.store(true);
        });
        timer.detach();

        Search::SearchResult result = searcher_.search(board_, msTime, stop_);
        if (result.bestMove == Moves::NULL_MOVE) {
            MoveList moves;
            Movegen::generateLegalMoves(board_, moves);
            if (moves.size() > 0) {
                sendBestMove(moveToString(moves.moves[0]));
            } else {
                sendBestMove("0000");
            }
        } else {
            sendBestMove(moveToString(result.bestMove));
        }
    });
}

void UCI::handleStop() {
    stop_.store(true);
    if (searchThread_.joinable()) {
        searchThread_.join();
    }
}

void UCI::send(const std::string &msg) {
    std::lock_guard<std::mutex> lock(outputMutex_);
    std::cout << msg << "\n";
    std::cout.flush();
}

void UCI::sendBestMove(const std::string &move) {
    send("bestmove " + move);
}

std::vector<std::string> UCI::split(const std::string &line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    while(iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string UCI::moveToString(Move m) {
    auto squareStr = [](Square sq) -> std::string {
        std::string s;
        s += (char)('a' + fileOf(sq));
        s += (char)('1' + rankOf(sq));
        return s;
    };

    std::string str = squareStr(Moves::getFrom(m)) + squareStr(Moves::getTo(m));

    if (Moves::isPromotion(m)) {
        switch (Moves::getPromoPt(m)) {
            case QUEEN: str += 'q'; break;
            case ROOK: str += 'r'; break;
            case BISHOP: str += 'b'; break;
            case KNIGHT: str += 'n'; break;
            default: break;
        }
    }

    return str;
}

Move UCI::stringToMove(const std::string &str) {
    if (str.size() < 4) return Moves::NULL_MOVE;

    Square from = makeSquare(str[0] - 'a', str[1] - '1');
    Square to = makeSquare(str[2] - 'a', str[3] - '1');

    PieceType promoPt = QUEEN;
    bool isPromotion = false;
    if (str.size() == 5) {
        isPromotion = true;
        switch (str[4]) {
            case 'q': promoPt = QUEEN; break;
            case 'r': promoPt = ROOK; break;
            case 'b': promoPt = BISHOP; break;
            case 'n': promoPt = KNIGHT; break;
        }
    }

    Piece p = board_.getPieceOn(from);
    if (p == NO_PIECE) return Moves::NULL_MOVE;
    PieceType pt = typeOf(p);

    Moves::MoveFlag flag = Moves::NORMAL;
    if (isPromotion) {
        flag = Moves::PROMOTION;
    } else if (pt == PAWN && abs(rankOf(from) - rankOf(to)) == 2) {
        flag = Moves::DOUBLE_PAWN_PUSH;
    } else if (pt == KING && abs(fileOf(from) - fileOf(to)) == 2) {
        flag = (fileOf(to) == 6) ? Moves::CASTLE_KING : Moves::CASTLE_QUEEN;
    } else if (pt == PAWN && to == board_.getEnPassantSquare()) {
        flag = Moves::EN_PASSANT;
    }

    return Moves::createMove(from, to, pt, flag, promoPt);
}


}