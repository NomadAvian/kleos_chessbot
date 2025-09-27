#pragma once

#include "common.h"
#include "bitboards.h"
#include "attacks.h"
#include "zobrist_hash.h"

/*
    moves can be encoded into a 32-bit integer

    source square    : 0000 0000 0000 0000 0000 0000 0011 1111 ->
    target square    : 0000 0000 0000 0000 0000 1111 1100 0000 ->
    moved piece      : 0000 0000 0000 0000 1111 0000 0000 0000 ->
    promoted piece   : 0000 0000 0000 1111 0000 0000 0000 0000 ->
    capture flag     : 0000 0000 0001 0000 0000 0000 0000 0000 ->
    double push flag : 0000 0000 0010 0000 0000 0000 0000 0000 ->
    en passant flag  : 0000 0000 0100 0000 0000 0000 0000 0000 ->
    castling flag    : 0000 0000 1000 0000 0000 0000 0000 0000 -> 
    
    unused bits      : 1111 1111 0000 0000 0000 0000 0000 0000

    unused bits could be used in the future (?)
*/

/* ENCODE A MOVE */

/// @brief encode a move into a 32-bit integer
/// @param from source square
/// @param to target square
/// @param piece moved piece
/// @param promotedPiece blank if none
/// @param capture 
/// @param doublePush 
/// @param enPassant 
/// @param castling 
/// @return 
inline int encodeMove(int from,
                      int to,
                      int piece,
                      int promotedPiece,
                      int capture,
                      int doublePush,
                      int enPassant,
                      int castling
                    ) {
    return (from)                |
           (to << 6)             |
           (piece << 12)         |
           (promotedPiece << 16) |
           (capture << 20)       |
           (doublePush << 21)    |
           (enPassant << 22)     |
           (castling << 23);
}

/* DECODE A MOVE */

// get source square of a move
inline int getFromSquare(int move) {
    return move & 0x3f;
}

// get target square of a move
inline int getToSquare(int move) {
    return (move & 0xfc0) >> 6;
}

// get the moved piece
inline int getPiece(int move) {
    return (move & 0xf000) >> 12;
}

// get the promoted piece of a move if any
inline int getPromotedPiece(int move) {
    return (move & 0xf0000) >> 16;
}

// check if a move is a capture
inline bool isCapture(int move) {
    return move & 0x100000;
}

// check if a move is a double pawn push
inline bool isDoublePush(int move) {
    return move & 0x200000;
}

// check if a move is en passant
inline bool isEnpass(int move) {
    return move & 0x400000;
}

// check if a move is castle
inline bool isCastle(int move) {
    return move & 0x800000;
}

/* MOVELIST */

/// @brief data structure to hold list of psuedo-legal moves in a position
struct MoveList {
    int moves[256];
    int count;
    MoveList() : count(0) {}
};

/// @brief add a move to a move list
/// @param move_list 
/// @param move 
inline void addMove(MoveList &moveList, int move) {
    moveList.moves[moveList.count++] = move;
}

/* MAKE MOVES */

/// @brief try to make a given move on the main board
/// @param move 
/// @param captures flag for only playing capture moves
/// @return move is legal and played
inline bool makeMove(int move, bool captures) {
    // apply capture filter
    if (captures && !isCapture(move))
        return 0;
    // copy board state
    Board temp_board = board;

    int  from          = getFromSquare(move),
         to            = getToSquare(move),
         piece         = getPiece(move),
         promotedPiece = getPromotedPiece(move);
    bool capture      = isCapture(move),
         doublePush   = isDoublePush(move),
         enpass       = isEnpass(move),
         castle       = isCastle(move),
         side         = board.side;
    
    // move piece
    popBit(board.bb[piece], from);
    popBit(board.occ[side], from);
    board.hashKey ^= Zobrist::pieceKeys[piece][from];

    // handle captures
    if (enpass) {
        if (side == WHITE) {
            popBit(board.bb[p], to + 8);
            popBit(board.occ[BLACK], to + 8);
            board.hashKey ^= Zobrist::pieceKeys[p][to + 8];
        } else {
            popBit(board.bb[P], to - 8);
            popBit(board.occ[WHITE], to - 8);
            board.hashKey ^= Zobrist::pieceKeys[P][to - 8];
        }
    } else if (capture) {
        for (int bb_piece = (side == WHITE ? p : P); bb_piece <= (side == WHITE ? k : K); bb_piece++) {
            if(getBit(board.bb[bb_piece], to)) {
                popBit(board.bb[bb_piece], to);
                popBit(board.occ[side ^ 1], to);
                board.hashKey ^= Zobrist::pieceKeys[bb_piece][to];
                break;
            }
        }
    }

    // handle promotions
    if (promotedPiece != blank) {
        setBit(board.bb[promotedPiece], to);
        board.hashKey ^= Zobrist::pieceKeys[promotedPiece][to];
    } else {
        setBit(board.bb[piece], to);
        board.hashKey ^= Zobrist::pieceKeys[piece][to];
    }

    setBit(board.occ[side], to);

    // reset en passant square
    if (board.enpass != no_sq)
        board.hashKey ^= Zobrist::enpassantKeys[board.enpass];
    
    // handle double pawn pushes
    if (doublePush) {
        if (side == WHITE) {
            board.enpass = to + 8;
            board.hashKey ^= Zobrist::enpassantKeys[to + 8];
        } else {
            board.enpass = to - 8;
            board.hashKey ^= Zobrist::enpassantKeys[to - 8];
        }
    } else {
        board.enpass = no_sq;
    }

    // handle castling
    if (castle) {
        switch (to) {
            case (g1): // white king side castle
                popBit(board.bb[R], h1);
                popBit(board.occ[WHITE], h1);
                board.hashKey ^= Zobrist::pieceKeys[R][h1];
                setBit(board.bb[R], f1);
                setBit(board.occ[WHITE], f1);
                board.hashKey ^= Zobrist::pieceKeys[R][f1];
                break;
            case (c1): // white queen side castle
                popBit(board.bb[R], a1);
                popBit(board.occ[WHITE], a1);
                board.hashKey ^= Zobrist::pieceKeys[R][a1];
                setBit(board.bb[R], d1);
                setBit(board.occ[WHITE], d1);
                board.hashKey ^= Zobrist::pieceKeys[R][d1];
                break;
            case (g8): // black king side castle
                popBit(board.bb[r], h8);
                popBit(board.occ[BLACK], h8);
                board.hashKey ^= Zobrist::pieceKeys[r][h8];
                setBit(board.bb[r], f8);
                setBit(board.occ[BLACK], f8);
                board.hashKey ^= Zobrist::pieceKeys[r][f8];
                break;
            case (c8): // black queen side castle
                popBit(board.bb[r], a8);
                popBit(board.occ[BLACK], a8);
                board.hashKey ^= Zobrist::pieceKeys[r][a8];
                setBit(board.bb[r], d8);
                setBit(board.occ[BLACK], d8);
                board.hashKey ^= Zobrist::pieceKeys[r][d8];
                break;
        }
    }

    // update castling rights
    board.hashKey ^= Zobrist::castleKeys[board.castle];

    board.castle &= castlingRights[from];
    board.castle &= castlingRights[to];

    board.hashKey ^= Zobrist::castleKeys[board.castle];

    // update occupancies
    board.occ[BOTH] = board.occ[WHITE] | board.occ[BLACK];

    // change side
    board.side ^= 1;
    board.hashKey ^= Zobrist::sideKey;

    // check for checks
    if (Attacks::isSquareAttacked(((side == WHITE) ? getLS1B(board.bb[K]) : getLS1B(board.bb[k])), side ^ 1)) {
        board = temp_board;
        return 0; // make move failure
    }
    return 1; // make move success
}

/// @brief print a move in UCI format
/// @param move 
void printMove(int move);

/// @brief print all moves in a move list
/// @param moveList 
void printMoveList(MoveList moveList);