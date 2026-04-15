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
    Color enemy = ~sideToMove;


    //Save Boardstate in History
    history[historyIndex].lastMove = move;
    history[historyIndex].enPassantSquare = enPassantSquare;
    history[historyIndex].castlingRights = castlingRights;
    history[historyIndex].halfMoveClock = halfMoveClock;
    history[historyIndex].capturedPiece = pieceOn[to];
    history[historyIndex].zobristHash = zobristHash;
    historyIndex++;

    zobristHash ^= Zobrist::pieceKeys[sideToMove][pt][from];
    zobristHash ^= Zobrist::pieceKeys[sideToMove][pt][to];
    zobristHash ^= Zobrist::castlingKeys[castlingRights];

    if (enPassantSquare != NO_SQUARE) {
        zobristHash ^= Zobrist::enPassantKeys[enPassantSquare % 8]; //Remove old en passant square from hash
    }

    //Remove opponent Piece in Bitboard
    if (pieceOn[to] != NO_PIECE) {
        pieces[enemy][pieceOn[to]] &= ~squareBB(to);
        zobristHash ^= Zobrist::pieceKeys[enemy][pieceOn[to]][to]; //Remove captured piece from hash
    }

    //Move own Piece in Bitboard
    pieces[sideToMove][pt] |= squareBB(to);
    pieces[sideToMove][pt] &= ~squareBB(from);

    //Array update
    pieceOn[to] = pieceOn[from];
    pieceOn[from] = NO_PIECE;

    //Flag Handling
    if (Moves::isEnPassant(move)) {
        Square capturedPawnSq = static_cast<Square>(to + (sideToMove == WHITE ? -8 : 8));
        pieceOn[capturedPawnSq] = NO_PIECE;
        pieces[enemy][PAWN] &= ~squareBB(capturedPawnSq);

        zobristHash ^= Zobrist::pieceKeys[enemy][PAWN][capturedPawnSq]; //Remove captured pawn from hash
    } else if (Moves::isCastleKing(move)) {
        Square rookFrom = sideToMove == WHITE ? H1 : H8;
        Square rookTo   = sideToMove == WHITE ? F1 : F8;
        pieces[sideToMove][ROOK] |=  squareBB(rookTo);
        pieces[sideToMove][ROOK] &= ~squareBB(rookFrom);
        pieceOn[rookTo]   = makePiece(sideToMove, ROOK);
        pieceOn[rookFrom] = NO_PIECE;

        zobristHash ^= Zobrist::pieceKeys[sideToMove][ROOK][sideToMove == WHITE ? H1 : H8]; //Remove rook from old square
        zobristHash ^= Zobrist::pieceKeys[sideToMove][ROOK][sideToMove == WHITE ? F1 : F8]; //Add rook to new square
    } else if (Moves::isCastleQueen(move)) {
        Square rookFrom = sideToMove == WHITE ? A1 : A8;
        Square rookTo   = sideToMove == WHITE ? D1 : D8;
        pieces[sideToMove][ROOK] |=  squareBB(rookTo);
        pieces[sideToMove][ROOK] &= ~squareBB(rookFrom);
        pieceOn[rookTo]   = makePiece(sideToMove, ROOK);
        pieceOn[rookFrom] = NO_PIECE;

        zobristHash ^= Zobrist::pieceKeys[sideToMove][ROOK][sideToMove == WHITE ? A1 : A8]; //Remove rook from old square
        zobristHash ^= Zobrist::pieceKeys[sideToMove][ROOK][sideToMove == WHITE ? D1 : D8]; //Add rook to new square
    } else if (Moves::isPromotion(move)) {
        PieceType promoPt = Moves::getPromoPt(move);
        pieces[sideToMove][PAWN]   &= ~squareBB(to);
        pieces[sideToMove][promoPt] |=  squareBB(to);
        pieceOn[to] = makePiece(sideToMove, promoPt);

        zobristHash ^= Zobrist::pieceKeys[sideToMove][PAWN][to]; //Remove pawn from hash
        zobristHash ^= Zobrist::pieceKeys[sideToMove][promoPt][to]; //Add promoted piece to hash
    }

    //En Passant Handling
    if (Moves::isDoublePP(move)) {
        enPassantSquare = static_cast<Square>(to + (sideToMove == WHITE ? -8 : 8));
        zobristHash ^= Zobrist::enPassantKeys[enPassantSquare % 8]; //Add new en passant square to hash
    } else {
        enPassantSquare = NO_SQUARE;
    }

    //Castling Rights
    if (pt == KING) {
        castlingRights &= sideToMove == WHITE ? ~(CASTLE_WK | CASTLE_WQ) : ~(CASTLE_BK | CASTLE_BQ);
    }

    Piece captured = history[historyIndex - 1].capturedPiece;
    if (pt == ROOK || typeOf(captured) == ROOK) {
        if (from == A1 || to == A1) castlingRights &= ~CASTLE_WQ;
        else if (from == H1 || to == H1) castlingRights &= ~CASTLE_WK;
        else if (from == A8 || to == A8) castlingRights &= ~CASTLE_BQ;
        else if (from == H8 || to == H8) castlingRights &= ~CASTLE_BK;
    }

    zobristHash ^= Zobrist::castlingKeys[castlingRights]; //Add new castling rights to hash

    //Clocks
    if (pt == PAWN || captured != NO_PIECE) {
        halfMoveClock = 0;
    } else {
        halfMoveClock++;
    }

    if (sideToMove == BLACK) fullMoveClock++;

    sideToMove = ~sideToMove;
    zobristHash ^= Zobrist::sideKey; //Change side to move in hash

    updateOccupancies();
}

void Board::unmakeMove() {
    historyIndex--;
    sideToMove = ~sideToMove;

    const BoardState &state = history[historyIndex];
    const Move move = state.lastMove;

    Square from = Moves::getFrom(move);
    Square to = Moves::getTo(move);
    PieceType pt = Moves::getPt(move);
    Color enemy = ~sideToMove;

    //Castling Rights
    castlingRights = state.castlingRights;

    //En Passant Square
    enPassantSquare = state.enPassantSquare;

    halfMoveClock = state.halfMoveClock;

    if (sideToMove == WHITE) {
        fullMoveClock--;
    }

    //Array Update
    pieceOn[from] = makePiece(sideToMove, pt);
    pieceOn[to] = NO_PIECE;

    //Bitboard Update
    pieces[sideToMove][pt] |= squareBB(from);
    pieces[sideToMove][pt] &= ~squareBB(to);

    if (state.capturedPiece != NO_PIECE) {
        pieces[enemy][typeOf(state.capturedPiece)] |= squareBB(to);
        pieceOn[to] = state.capturedPiece;
    }

    //Flag Handling
    if (Moves::isEnPassant(move)) {
        Square capturedPawnSq = static_cast<Square>(to + (sideToMove == WHITE ? -8 : 8));
        pieceOn[capturedPawnSq] = makePiece(enemy, PAWN);
        pieces[enemy][PAWN] |= squareBB(capturedPawnSq);
    } else if (Moves::isCastleKing(move)) {
        Square rookFrom = sideToMove == WHITE ? F1 : F8;
        Square rookTo   = sideToMove == WHITE ? H1 : H8;
        pieces[sideToMove][ROOK] |=  squareBB(rookTo);
        pieces[sideToMove][ROOK] &= ~squareBB(rookFrom);
        pieceOn[rookTo]   = makePiece(sideToMove, ROOK);
        pieceOn[rookFrom] = NO_PIECE;
    } else if (Moves::isCastleQueen(move)) {
        Square rookFrom = sideToMove == WHITE ? D1 : D8;
        Square rookTo   = sideToMove == WHITE ? A1 : A8;
        pieces[sideToMove][ROOK] |=  squareBB(rookTo);
        pieces[sideToMove][ROOK] &= ~squareBB(rookFrom);
        pieceOn[rookTo]   = makePiece(sideToMove, ROOK);
        pieceOn[rookFrom] = NO_PIECE;
    } else if (Moves::isPromotion(move)) {
        pieces[sideToMove][Moves::getPromoPt(move)] &= ~squareBB(to);
    }

    updateOccupancies();
    zobristHash = state.zobristHash; //Restore hash from history
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