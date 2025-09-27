#include "evaluation.h"

const int materialScore[12] = { 
     100,   // white pawn
     320,   // white knight
     330,   // white bishop
     500,   // white rook
     900,   // white queen
     0,     // white king
     100,   // black pawn
     320,   // black knight
     330,   // black bishop
     500,   // black rook
     900,   // black queen
     0      // black king
};

/* POSITIONAL SCORES */

const int pawnScore[64] = {
    90,  90,  90,  90,  90,  90,  90,  90,
    30,  30,  30,  40,  40,  30,  30,  30,
    20,  20,  20,  30,  30,  30,  20,  20,
    10,  10,  10,  20,  20,  10,  10,  10,
     5,   5,  10,  20,  20,   5,   5,   5,
     0,   0,   0,   5,   5,   0,   0,   0,
     0,   0,   0, -10, -10,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0
};

const int knightScore[64] = {
    -15,   0,   0,   0,   0,   0,   0,  -15,
     -5,   0,   0,  10,  10,   0,   0,   -5,
     -5,   5,  20,  20,  20,  20,   5,   -5,
     -5,  10,  20,  30,  30,  20,  10,   -5,
     -5,  10,  20,  30,  30,  20,  10,   -5,
     -5,   5,  20,  10,  10,  20,   5,   -5,
     -5,   0,   0,   0,   0,   0,   0,   -5,
    -15, -10,   0,   0,   0,   0, -10,  -15
};

const int bishopScore[64] = {
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   0,  10,  10,   0,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,  10,   0,   5,   5,   0,  10,   0,
     0,  20,   0,   5,   5,   0,  20,   0,
     0,   0, -10,   0,   0, -10,   0,   0
};

const int rookScore[64] = {
    50,  50,  50,  50,  50,  50,  50,  50,
    50,  50,  50,  50,  50,  50,  50,  50,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,  10,  20,  20,  10,   0,   0,
     0,   0,   0,  20,  20,   0,   0,   0
};

const int kingScore[64] = {
     0,   0,   0,   0,   0,   0,   0,   0,
     0,   0,   5,   5,   5,   5,   0,   0,
     0,   5,   5,  10,  10,   5,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   5,  10,  20,  20,  10,   5,   0,
     0,   0,   5,  10,  10,   5,   0,   0,
     0,   5,   5,  -5,  -5,   0,   5,   0,
     0,   0,   5,   0, -15,   0,  10,   0
};

const int mirrorSquare[128] = {
	a1, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8
};

/* PENALTIES & BONUSES */

const int doubledPawnPenalty     = 10;
const int isolatedPawnPenalty    = 10;

const int passedPawnBonus[8]     = { 0, 5, 10, 30, 50, 75, 150, 200 };
const int protectedPawnBonus     = 10;

const int semiOpenFileScore      = 10;
const int openFileScore          = 15;

const int knightMobility         = 8;
const int bishopMobility         = 10;
const int rookMobility           = 8;
const int queenMobility          = 3;

const int bishopPairBonus        = 30;
const int rookSeventhRank        = 30;

/* FILE & RANK MASKS */

U64 fileMasks[64];
U64 rankMasks[64];
U64 isolatedPawnMasks[64];
U64 passedPawnMasks[2][64];

// rank index of squares
const int rankOfSquare[64] = {
     7, 7, 7, 7, 7, 7, 7, 7,
     6, 6, 6, 6, 6, 6, 6, 6,
     5, 5, 5, 5, 5, 5, 5, 5,
     4, 4, 4, 4, 4, 4, 4, 4,
     3, 3, 3, 3, 3, 3, 3, 3,
     2, 2, 2, 2, 2, 2, 2, 2,
     1, 1, 1, 1, 1, 1, 1, 1,
     0, 0, 0, 0, 0, 0, 0, 0
};

// file index of squares
const int fileOfSqure[64] = {
     0, 1, 2, 3, 4, 5, 6, 7,
     0, 1, 2, 3, 4, 5, 6, 7,
     0, 1, 2, 3, 4, 5, 6, 7,
     0, 1, 2, 3, 4, 5, 6, 7,
     0, 1, 2, 3, 4, 5, 6, 7,
     0, 1, 2, 3, 4, 5, 6, 7,
     0, 1, 2, 3, 4, 5, 6, 7,
     0, 1, 2, 3, 4, 5, 6, 7
};

U64 Evaluation::setFileRankMasks(int file, int rank) {
     U64 mask = 0ULL;
     for (int r = 0; r < 8; r++) { // ranks
          for(int f = 0; f < 8; f++) { // files
               int sq = (r << 3) + f;
               if (file != -1 && f == file)
                    setBit(mask, sq);
               else if (rank != -1 && r == rank)
                    setBit(mask, sq);
          }
     }
     return mask;
}

void Evaluation::initializeEvaluationMasks() {
     // file, rank masks
     for (int r = 0; r < 8; r++) {
          for(int f = 0; f < 8; f++) {
               int sq = (r << 3) + f;
               fileMasks[sq] |= Evaluation::setFileRankMasks(f, -1);
               rankMasks[sq] |= Evaluation::setFileRankMasks(-1, r);
          }
     }
     // isolated pawn masks
     for (int r = 0; r < 8; r++) {
          for(int f = 0; f < 8; f++) {
               int sq = (r << 3) + f;
               isolatedPawnMasks[sq] |= setFileRankMasks(f - 1, -1);
               isolatedPawnMasks[sq] |= setFileRankMasks(f + 1, -1);
          }
     }
     // passed pawn masks
     for (int r = 0; r < 8; r++) {
          for(int f = 0; f < 8; f++) {
               int sq = (r << 3) + f;
               // white
               passedPawnMasks[WHITE][sq] |= Evaluation::setFileRankMasks(f - 1, -1);
               passedPawnMasks[WHITE][sq] |= Evaluation::setFileRankMasks(f, -1);
               passedPawnMasks[WHITE][sq] |= Evaluation::setFileRankMasks(f + 1, -1);
               for (int i = 0; i < (8 - r); i++)
                    passedPawnMasks[WHITE][sq] &= ~rankMasks[(7 - i) * 8 + f];
               // black
               passedPawnMasks[BLACK][sq] |= Evaluation::setFileRankMasks(f - 1, -1);
               passedPawnMasks[BLACK][sq] |= Evaluation::setFileRankMasks(f, -1);
               passedPawnMasks[BLACK][sq] |= Evaluation::setFileRankMasks(f + 1, -1);
               for (int i = 0; i <= r; i++)
                    passedPawnMasks[BLACK][sq] &= ~rankMasks[i * 8 + f];
          }
     }
}