#pragma once

#include "common.h"

/*
    Bitboards are unsigned 64-bit integers where
    each bit corresponds to a square on the chess board.
    They are used to represent occupancies & attack masks
*/

/// @brief 0x88 board representation data structure
struct Board {
    U64 bb[12];   // piece bitboards (indexed by Piece enum)
    U64 occ[3];   // side occupancy bitboards (indexed by Side enum)
    int side;     // side to move
    int castle;   // castling rights
    int enpass;   // en passant square
    U64 hashKey; // zobrist hash key
};

/* GLOBAL BITBOARDS & BOARD STATE VARIABLES */

extern int ply;                   // half move counter

extern Board board;               // main board
extern U64 repetitionTable[1000]; // table storing hash keys of repeated positions
extern int repetitionIndex;       // index of latest repetition entry

/* BITBOARD OPERATIONS */

// check if a bit is set in a bitboard
inline bool getBit(U64 bb, int sq) {
    return (bb >> sq) & 1ULL;
}

// set a bit at a given square
inline void setBit(U64 &bb, int sq) {
    bb |= 1ULL << sq;
}

// clear a bit at a given square
inline void popBit(U64 &bb, int sq) {
    bb &= ~(1ULL << sq);
}

// count number of set bits in a bitboard
inline int countBits(U64 bb) {
    return __builtin_popcountll(bb);
}

// get index of least significant 1 bit in a bitboard
inline int getLS1B(U64 bb) {
    return (bb) ? __builtin_ctzll(bb) : no_sq;
}

// get least significant 1 bit and clear it
inline int popLS1B(U64 &bb) {
    int id = getLS1B(bb);
    popBit(bb, id);
    return id;
}

/* functions */

/// @brief reset the game board
void reset();

/// @brief pretty print a bitboard
/// @param bb 
void printBitboard(U64 bb);

/// @brief pretty print the main board
/// @param board 
void printBoard();