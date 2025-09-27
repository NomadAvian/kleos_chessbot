#pragma once

#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <chrono>

// required for time control code
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/select.h>
#endif

/*
    this chess engine was made primarily following tutorials by Code Monkey Games
    references: https://www.youtube.com/playlist?list=PLmN0neTso3Jxh8ZIylk74JpwfiWNI76Cs
*/

constexpr const char *NAME = "Kleos Classic";
constexpr const char *AUTHOR = "NomadAvian";

// bitboard datatype
using U64 = uint64_t;

// side enum
enum Side {
    WHITE, BLACK, BOTH
};

// piece enum
enum Piece {
    // white pieces
    P, N, B, R, Q, K,
    // black pieces
    p, n, b, r, q, k,
    // empty piece
    blank
};

// square enum
enum Squares {
    // board squares
    a8, b8, c8, d8, e8, f8, g8, h8,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a1, b1, c1, d1, e1, f1, g1, h1,
    // empty square
    no_sq
};

// castling right encodings
enum Castling {
    wk = 1, wq = 2, bk = 4, bq = 8
};

// for updating castling rights
constexpr int castlingRights[64] = {
    7, 15, 15, 15,  3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14
};

// ascii representation of pieces
constexpr const char *pieceToAscii = "PNBRQKpnbrqk.";

// board squares as strings (indexed by Squares enums)
constexpr const char *sqToCoords[64] = {
    "a8","b8","c8","d8","e8","f8","g8","h8",
    "a7","b7","c7","d7","e7","f7","g7","h7",
    "a6","b6","c6","d6","e6","f6","g6","h6",
    "a5","b5","c5","d5","e5","f5","g5","h5",
    "a4","b4","c4","d4","e4","f4","g4","h4",
    "a3","b3","c3","d3","e3","f3","g3","h3",
    "a2","b2","c2","d2","e2","f2","g2","h2",
    "a1","b1","c1","d1","e1","f1","g1","h1"
};

// for promoted piece printing in UCI format
constexpr const char *promotedPieces[13] = {
    "","n","b","r","q","",
    "","n","b","r","q","k",
    ""
};

// start FEN
constexpr const char *startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";

/* TIME CONTROL UTILITY */

inline int getTimeMs() {
    #ifdef _WIN32
        return GetTickCount();
    #else
        struct timeval time_value;
        gettimeofday(&time_value, NULL);
        return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
    #endif
}