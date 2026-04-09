#pragma once
#include "BitBoard.h"
#include <string>


namespace Thoth {

namespace Board {

constexpr int MAX_HISTORY = 1024;

enum CastlingRights : uint8_t {
    NO_CASTLE = 0b0000,
    CASTLE_WK = 0b0001,
    CASTLE_WQ = 0b0010,
    CASTLE_BK = 0b0100,
    CASTLE_BQ = 0b1000
};

class Board {
    private:
        BitBoard pieces[COLOR_NB][PIECE_TYPE_NB];
        BitBoard occupancies[COLOR_NB + 1];
        PieceType pieceOn[SQUARE_NB];
        uint64_t zobristHash;

        Color sideToMove = WHITE;
        Square enPassantSquare = NO_SQUARE;
        CastlingRights castlingRights = NO_CASTLE;
        int halfMoveClock = 0;
        int fullMoveClock = 1;

        struct BoardState {
            
        };

        BoardState history[MAX_HISTORY];
        int historyIndex = 0;

    public:
        Board();

        //Getter
        Square getEnPassantSquare() const {return enPassantSquare;}
        Color getSideToMove() const {return sideToMove;}
        CastlingRights getCastlingRights() const {return castlingRights;}
        BitBoard getPiece(Color c, PieceType p) const {return pieces[c][p];}
        BitBoard getPieceOn(Square sq) const {return pieceOn[sq];}
        BitBoard getOccupancy(Color c) const {return occupancies[c];}
        uint64_t getZobristHash() const {return zobristHash;}
        int getHistoryIndex() const {return historyIndex;}


        //Methods
        bool isInCheck(Color side) const;
        bool isSquareAttacked(Square sq, Color attackingSide) const;

        void updateOccupanies();
        void computeZobristHash();

        void clearBoard();
        void parseFEN(const std::string &fen);

        //void makeMove(Move move);
        //void unmakeMove();

        void printBoard() const;
        void printBitBoard(BitBoard bb) const;
};

}
}