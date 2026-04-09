#include "Board.h"
#include <vector>
#include <sstream>
#include <iostream>


namespace Thoth {


constexpr PieceType charToPieceType(char c) {
    switch(c) {
            case 'p': return PAWN;
            case 'n': return KNIGHT;
            case 'b': return BISHOP;
            case 'r': return ROOK;
            case 'q': return QUEEN;
            case 'k': return KING;
            default: return PIECE_TYPE_NB;
        }
}





void Board::updateOccupancies() {
    occupancies[WHITE] = 0ULL;
    occupancies[BLACK] = 0ULL;
    
    for (int i = 0; i < PIECE_TYPE_NB; i++) {
        occupancies[WHITE] |= pieces[WHITE][i];
        occupancies[BLACK] |= pieces[BLACK][i];
    }

    occupancies[COLOR_NB] = occupancies[WHITE] | occupancies[BLACK];
}

void Board::clearBoard() {
    pieces = {};
    occupancies = {};
    pieceOn.fill(NO_PIECE);

    zobristHash = 0;
    enPassantSquare = NO_SQUARE;
    castlingRights = NO_CASTLE;
    sideToMove = WHITE;
    halfMoveClock = 0;
    fullMoveClock = 1;
    historyIndex = 0;
}

void Board::parseFEN(const std::string &fen) {
    std::istringstream ss(fen);
    std::string token;
    PieceType pieceType;
    Color color;

    clearBoard();

    //Piece Placement
    ss >> token;
    int file = 0, rank = 7;
    for (char c : token) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (c >= '1' && c <= '8') {
            file += c - '0';
        } else {
            Color c = isupper(c) ? WHITE : BLACK;
            PieceType pieceType = charToPieceType(c);

            if (pieceType != PIECE_TYPE_NB) {
                Square sq = makeSquare(file, rank);
                pieces[c][pieceType] |= squareBB(sq);
                pieceOn[sq] = makePiece(c, pieceType);
                file++;
            }
        }
    }

    updateOccupancies();

    //Sido to Move
    ss >> token;
    sideToMove = (token[0] == 'w') ? WHITE : BLACK;

    //Castling Rights
    ss >> token;
    if (token[0] != '-') {
        for (char c : token) {
            switch(c) {
                case 'K':
                    castlingRights |= CASTLE_WK;
                    break;
                case 'Q':
                    castlingRights |= CASTLE_WQ;
                    break;
                case 'k':
                    castlingRights |= CASTLE_BK;
                    break;
                case 'q':
                    castlingRights |= CASTLE_BQ;
                    break;
            }
        }
    } else {
        castlingRights = NO_CASTLE;
    }

    //En Passant
    ss >> token;
    if(token[0] != '-') {
        int file = token[0] - 'a';
        int rank = token[1] - '1';
        enPassantSquare = makeSquare(file, rank);
    }

    
    //Move Clock
    if (ss >> token) halfMoveClock = stoi(token);
    if (ss >> token) fullMoveClock = stoi(token); 

    //computeZobristHash();
}

void Board::printBoard() const {
    BitBoard occupied = getOccupancy(COLOR_NB);

    for (int rank = 7; rank >= 0; rank--) {
        std::cout << rank + 1 << " ";
        for (int file = 0; file < 8; file++) {
            Square sq = makeSquare(file, rank);
            std::cout << ((occupied >> sq) & 1) << " ";
        }

        std::cout << std::endl;
    }

    std::cout << " A B C D E F G H" << std::endl;
}

}