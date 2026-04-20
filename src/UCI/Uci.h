#pragma once
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include "../Board-Representation/Board.h"
#include "../Movegeneration/Movegen.h"

namespace Thoth {

class UCI {
    private:
        void handleUci();
        void handleIsReady();
        void handleUciNewGame();
        void handlePosition(const std::vector<std::string> &tokens);
        void handleGo(const std::vector<std::string> &tokens);
        void handleStop();

        void send(const std::string &msg);
        void sendBestMove(const std::string &move);

        std::vector<std::string> split(const std::string &line);
        std::string moveToString(Move m);
        Move stringToMove(const std::string &str);

        Board board_;
        std::thread searchThread_;
        std::atomic<bool> stop_{false};
        std::mutex outputMutex_;


    public:
        UCI();
        void loop();
};



}