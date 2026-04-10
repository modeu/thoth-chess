#pragma once
#include "BitBoard.h"
#include "ZobristKeys.h"
#include <string>
#include <array>


namespace Thoth {

constexpr int MAX_HISTORY = 1024;

enum CastlingRights : uint8_t {
    NO_CASTLE = 0b0000,
    CASTLE_WK = 0b0001,
    CASTLE_WQ = 0b0010,
    CASTLE_BK = 0b0100,
    CASTLE_BQ = 0b1000
};

constexpr CastlingRights operator|(CastlingRights a, CastlingRights b) {
    return static_cast<CastlingRights>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

constexpr CastlingRights operator&(CastlingRights a, CastlingRights b) {
    return static_cast<CastlingRights>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

constexpr CastlingRights& operator|=(CastlingRights& a, CastlingRights b) {
    return a = a | b;
}

constexpr CastlingRights& operator&=(CastlingRights& a, CastlingRights b) {
    return a = a & b;
}

class Board {
    private:
        std::array<std::array<BitBoard, PIECE_TYPE_NB>, COLOR_NB> pieces;
        std::array<BitBoard, COLOR_NB + 1> occupancies;
        std::array<Piece, SQUARE_NB> pieceOn;
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
        //Board();

        //Getter
        Square getEnPassantSquare() const {return enPassantSquare;}
        Color getSideToMove() const {return sideToMove;}
        CastlingRights getCastlingRights() const {return castlingRights;}
        BitBoard getPiece(Color c, PieceType p) const {return pieces[c][p];}
        Piece getPieceOn(Square sq) const {return pieceOn[sq];}
        BitBoard getOccupancy(Color c) const {return occupancies[c];}
        uint64_t getZobristHash() const {return zobristHash;}
        int getHistoryIndex() const {return historyIndex;}


        //Methods
        //bool isInCheck(Color side) const;
        //bool isSquareAttacked(Square sq, Color attackingSide) const;

        void updateOccupancies();
        void computeZobristHash();

        void clearBoard();
        void parseFEN(const std::string &fen);

        //void makeMove(Move move);
        //void unmakeMove();

        void printBoard() const;
        void printBitBoard(BitBoard bb) const;
};
}