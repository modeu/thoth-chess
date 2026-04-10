#include "Board.h"
#include <sstream>
#include <iostream>


namespace Thoth {

//Helper Functions
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


//Board Class
void Board::updateOccupancies() {
    occupancies[WHITE] = 0ULL;
    occupancies[BLACK] = 0ULL;
    
    for (int i = 0; i < PIECE_TYPE_NB; i++) {
        occupancies[WHITE] |= pieces[WHITE][i];
        occupancies[BLACK] |= pieces[BLACK][i];
    }

    occupancies[COLOR_NB] = occupancies[WHITE] | occupancies[BLACK];
}

void Board::computeZobristHash() {
    zobristHash = 0ULL;

    for (int c = 0; c < COLOR_NB; c++) {
        for (int pt = 0; pt < PIECE_TYPE_NB; pt++) {
            BitBoard bb = pieces[c][pt];

            while(bb) {
                Square sq = static_cast<Square>(popLSB(bb));
                zobristHash ^= Zobrist::pieceKeys[c][pt][sq]; 
            }
        }
    }

    if (enPassantSquare != NO_SQUARE) {
        zobristHash ^= Zobrist::enPassantKeys[enPassantSquare % 8];
    }

    zobristHash ^= Zobrist::castlingKeys[castlingRights];

    if (sideToMove == BLACK) zobristHash ^= Zobrist::sideKey;
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
            Color col = isupper(c) ? WHITE : BLACK;
            PieceType pieceType = charToPieceType(tolower(c));

            if (pieceType != PIECE_TYPE_NB) {
                Square sq = makeSquare(file, rank);
                pieces[col][pieceType] |= squareBB(sq);
                pieceOn[sq] = makePiece(col, pieceType);
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

    computeZobristHash();
}

void Board::makeMove(Move move) {
    Square from = Moves::getFrom(move);
    Square to = Moves::getTo(move);
    PieceType pt = Moves::getPt(move);


    //Save Boardstate in History
    history[historyIndex].lastMove = move;
    history[historyIndex].enPassantSquare = enPassantSquare;
    history[historyIndex].castlingRights = castlingRights;
    history[historyIndex].halfMoveClock = halfMoveClock;
    history[historyIndex].capturedPiece = pieceOn[to];
    history[historyIndex].zobristHash = zobristHash;
    historyIndex++;

    //Remove opponent Piece in Bitboard
    if (pieceOn[to] != NO_PIECE) {
        pieces[!sideToMove][pieceOn[to]] &= ~(1ULL << to);
    }

    //Move own Piece in Bitboard
    pieces[sideToMove][pt] |= 1ULL << to;
    pieces[sideToMove][pt] &= ~(1ULL << from);

    //Array update
    pieceOn[to] = pieceOn[from];
    pieceOn[from] = NO_PIECE;

    //Flag Handling
    if (Moves::isEnPassant(move)) {
        int capturedPawnSq = to + (sideToMove == WHITE ? -8 : 8);
        pieceOn[capturedPawnSq] = NO_PIECE;
        pieces[!sideToMove][PAWN] &= ~(1ULL << capturedPawnSq); 
    } else if (Moves::isCastleKing(move)) {
        pieces[sideToMove][ROOK] |=  1ULL << (sideToMove == WHITE ? F1 : F8);
        pieces[sideToMove][ROOK] &= ~(1ULL << (sideToMove == WHITE ? H1 : H8));
        pieceOn[sideToMove == WHITE ? F1 : F8] = ROOK;
        pieceOn[sideToMove == WHITE ? H1 : H8] = NO_PIECE;
    } else if (Moves::isCastleQueen(move)) {
        pieces[sideToMove][ROOK] |=  1ULL << (sideToMove == WHITE ? D1 : D8);
        pieces[sideToMove][ROOK] &= ~(1ULL << (sideToMove == WHITE ? A1 : A8));
        pieceOn[sideToMove == WHITE ? D1 : D8] = ROOK;
        pieceOn[sideToMove == WHITE ? A1 : A8] = NO_PIECE;
    } else if (Moves::isPromotion(move)) {
        pieces[sideToMove][PAWN] &= ~(1ULL << to);
        pieces[sideToMove][Moves::getPromoPt(move)] |= 1ULL << to;
        pieceOn[to] = Moves::getPromoPt(move);
    }
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

void Board::printBitBoard(BitBoard bb) const {
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << rank + 1 << " ";
        for (int file = 0; file < 8; file++) {
            Square sq = makeSquare(file, rank);
            std::cout << ((bb >> sq) & 1) << " ";
        }

        std::cout << std::endl;
    }

    std::cout << " A B C D E F G H" << std::endl;
}

}