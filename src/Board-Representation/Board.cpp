#include "Board.h"
#include <vector>
#include <sstream>
#include <iostream>


namespace Thoth {


void Board::updateOccupancies() {
    occupancies[WHITE] = 0ULL;
    occupancies[BLACK] = 0ULL;
    
    for (int i = 0; i < PIECE_TYPE_NB; i++) {
        occupancies[WHITE] |= pieces[WHITE][i];
        occupancies[BLACK] |= pieces[BLACK][i];
    }

    occupancies[COLOR_NB] = occupancies[WHITE] | occupancies[BLACK];
}

void Board::parseFEN(const std::string &fen) {
    std::vector<std::string> tokens;
    std::stringstream ss(fen);
    std::string token;
    int pieceType;
    int color;

    //clearBoard();

    while (getline(ss, token, ' ')) {
        if (!token.empty()) tokens.push_back(token);
    }


    //Piece Placement
    int file = 0, rank = 7;
    for (int i = 0; i < tokens[0].size(); i++) {
        char c = tokens[0][i];

        if (c == '/') {
            rank--;
            file = 0;
            continue;
        }

        if (c >= '0' && c <= '8') {
            file += c - '0';
            continue;
        }

        color = isupper(c) ? WHITE : BLACK;
        c = tolower(c);

        switch(c) {
            case 'p':
                pieceType = PAWN;
                break;
            case 'n':
                pieceType = KNIGHT;
                break;
            case 'b':
                pieceType = BISHOP;
                break;
            case 'r':
                pieceType = ROOK;
                break;
            case 'q':
                pieceType = QUEEN;
                break;
            case 'k':
                pieceType = KING;
                break;
        }

        pieces[color][pieceType] |= 1ULL << (rank*8 + file);
        file++;
    }

    updateOccupancies();

    //Sido to Move
    sideToMove = (tokens[1][0] == 'w') ? WHITE : BLACK;

    //Castling Rights
    if (tokens[2][0] != '-') {
        for (int i = 0; i < tokens[2].size(); i++) {
            char c = tokens[2][i];

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
    if(tokens[3][0] != '-') {
        int file = (int)(tokens[3][0] - 'a');
        int rank = (int)(tokens[3][1] - '1');
        enPassantSquare = makeSquare(file, rank);
    } else {
        enPassantSquare = NO_SQUARE;
    }

    
    //Move Clock
    halfMoveClock = stoi(tokens[4]);
    fullMoveClock = stoi(tokens[5]); 
    
    //pieceON fill
    for(int sq = 0; sq < 64; sq++) {
        pieceOn[sq] = NO_PIECE;
        for (int i = 0; i < PIECE_TYPE_NB; i++) {
            if (pieces[WHITE][i] & (1ULL << sq)) pieceOn[sq] = makePiece(WHITE, static_cast<PieceType>(i));
            if (pieces[BLACK][i] & (1ULL << sq)) pieceOn[sq] = makePiece(BLACK, static_cast<PieceType>(i));
        }
    }

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