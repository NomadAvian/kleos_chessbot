#pragma once

#include "common.h"
#include "bitboards.h"
#include "attacks.h"

/*
        static evaluation
*/

// evaluation constants
constexpr int inf       = 50000;       // upper bound of |eval score|
constexpr int mateScore = 48000;       // lower bound of checkmate score
constexpr int mateValue = 49000;       // absolute base value of evaluation mating position
constexpr int drawScore = 0;           // equal position evaluation score
constexpr int pvScore   = 20000;       // principle variation score
constexpr int killerMove1Score = 9000; // first killer move score
constexpr int killerMove2Score = 8000; // second killer move score 

// score tables
extern const int materialScore[12];    // material score for each piece
extern const int pawnScore[64];        // pawn positional score table
extern const int knightScore[64];      // knight positional score table
extern const int bishopScore[64];      // bishop positional score table
extern const int rookScore[64];        // rook positional score table
extern const int kingScore[64];        // king positional score table
extern const int mirrorSquare[128];     // mirror positional score table

// penalties & bonuses
extern const int doubledPawnPenalty;   // penalty for each doubled pawn
extern const int isolatedPawnPenalty;  // penalty for isolated pawns
extern const int passedPawnBonus[8];   // bonus for passed pawns (increases every rank)
extern const int protectedPawnBonus;   // bonus for protected pawns (increases every rank)

extern const int semiOpenFileScore;    // bonus for rooks on semi-open files
extern const int openFileScore;        // bonus for rooks on open files

extern const int knightMobility;       // mobility score modifier for knights
extern const int bishopMobility;       // mobility score modifier for bishop
extern const int rookMobility;         // mobility score modifier for rook
extern const int queenMobility;        // mobility score modifier for queen

extern const int bishopPairBonus;      // bonus for having both bishops
extern const int rookSeventhRank;      // bonus for having rooks on the opponents 2nd rank

// masks
extern U64 fileMasks[64];              // [square]
extern U64 rankMasks[64];              // [square]
extern U64 isolatedPawnMasks[64];      // [square]
extern U64 passedPawnMasks[2][64];     // [side][square]

extern const int rankOfSquare[64];
extern const int fileOfSqure[64];

namespace Evaluation {


    /* GENERATE MASKS */    

    /// @brief generate masks for files and ranks
    /// @param file 
    /// @param rank 
    /// @return 
    U64 setFileRankMasks(int file, int rank);

    /// @brief initialize all masks required for evaluation
    void initializeEvaluationMasks();
    
    /* MAIN EVALUATION FUNCTION */

    /// @brief evaluate the current position
    /// @return static evaluation score
    inline int evaluate() {
        int wScore = 0, bScore = 0;
        int piece, sq;
        // pawn score penalties & bonuses
        int doubledPawns, isolatedPawns, passedPawns;
        U64 bb;
        for (int bb_piece = P; bb_piece <= k; bb_piece++)
        {
            bb = board.bb[bb_piece];
            piece = bb_piece;
            while (bb) {
                sq = popLS1B(bb);
                // material score
                if (piece <= K)
                        wScore += materialScore[piece];
                else
                        bScore += materialScore[piece];
                // positional score
                switch (piece) {
                    // white pieces (+)
                    case P: {

                        // positional score
                        wScore += pawnScore[sq];

                        // doubled pawn penalties
                        doubledPawns = countBits(board.bb[P] & fileMasks[sq]);
                        if (doubledPawns > 1)
                                wScore -= doubledPawnPenalty * doubledPawns;

                        // isolated pawn penalties
                        if ((board.bb[P] & isolatedPawnMasks[sq]) == 0)
                                wScore -= isolatedPawnPenalty;

                        // passed pawn bonus
                        if ((board.bb[p] & passedPawnMasks[WHITE][sq]) == 0)
                                wScore += passedPawnBonus[rankOfSquare[sq]];

                        // protected pawn bonus (?)
                        // wScore += protectedPawnBonus *
                        //          countBits(board.bb[P] & pawnAttacks[WHITE][sq]);

                        break;
                    }
                    case N: {
                        // positional scores
                        wScore += knightScore[sq];

                        // mobility
                        wScore += (countBits(knightAttacks[sq] & ~board.occ[WHITE])) *
                                 knightMobility;

                        break;
                    }
                    case B: {

                        // positional scores
                        wScore += bishopScore[sq];

                        // mobility
                        wScore += countBits(Attacks::getBishopAttacks(sq, board.occ[BOTH])) *
                                 bishopMobility;

                        break;
                    }
                    case R: {

                        // positional score
                        wScore += rookScore[sq];

                        // semi open file bonus
                        if ((board.bb[P] & fileMasks[sq]) == 0)
                                wScore += semiOpenFileScore;

                        // open file bonus
                        if (((board.bb[P] | board.bb[p]) & fileMasks[sq]) == 0)
                                wScore += openFileScore;

                        // mobility
                        wScore += countBits(Attacks::getRookAttacks(sq, board.occ[BOTH])) *
                                 rookMobility;
                        
                        // 7th rank
                        if (rankOfSquare[sq] == 6)
                                wScore += rookSeventhRank;

                        break;
                    }
                    case Q: wScore += countBits(Attacks::getBishopAttacks(sq, board.occ[BOTH]) | Attacks::getRookAttacks(sq, board.occ[BOTH])) * 
                            queenMobility;
                            break;
                    case K: {

                        // positional score
                        wScore += kingScore[sq];

                        // semi open file penalty
                        if ((board.bb[P] & fileMasks[sq]) == 0)
                                wScore -= semiOpenFileScore;

                        // open file penalty
                        if (((board.bb[P] | board.bb[p]) & fileMasks[sq]) == 0)
                                wScore -= openFileScore;

                        // king safety
                        wScore += countBits(kingAttacks[sq] & board.occ[WHITE]) * 5;

                        break;
                    }
                    // black pieces (-)
                    case p: {

                        // positional score
                        bScore += pawnScore[mirrorSquare[sq]];
                        
                        // doubled pawn penalties
                        doubledPawns = countBits(board.bb[p] & fileMasks[sq]);
                        if (doubledPawns > 1)
                                bScore -= doubledPawnPenalty * doubledPawns;

                        // isolated pawn penalties
                        if ((board.bb[p] & isolatedPawnMasks[sq]) == 0)
                                bScore -= isolatedPawnPenalty;

                        // passed pawn bonus
                        if ((board.bb[P] & passedPawnMasks[BLACK][sq]) == 0)
                                bScore += passedPawnBonus[rankOfSquare[mirrorSquare[sq]]];

                        // protected pawn bonus (?)
                        // bScore += protectedPawnBonus *
                        //          countBits(board.bb[p] & pawnAttacks[BLACK][sq]);

                        break;
                    }
                    case n: {
                        
                        // positional score
                        bScore += knightScore[mirrorSquare[sq]];
                        
                        // mobility
                        bScore += (countBits(knightAttacks[sq] & ~board.occ[BLACK])) *
                                  knightMobility;

                        break;
                    }
                    case b: {

                        // positional bonus
                        bScore += bishopScore[mirrorSquare[sq]];
                        
                        // mobility
                        bScore += countBits(Attacks::getBishopAttacks(sq, board.occ[BOTH])) *
                                  bishopMobility;

                        break;
                    }
                    case r: {

                        // positional score
                        bScore += rookScore[mirrorSquare[sq]];

                        // semi open file bonus
                        if ((board.bb[p] & fileMasks[sq]) == 0)
                                bScore += semiOpenFileScore;

                        // open file bonus
                        if (((board.bb[P] | board.bb[p]) & fileMasks[sq]) == 0)
                                bScore += openFileScore;
                        
                        // mobility
                        bScore += countBits(Attacks::getRookAttacks(sq, board.occ[BOTH])) *
                                 rookMobility;
                                
                        // 7th rank
                        if (rankOfSquare[sq] == 1)
                                bScore += rookSeventhRank;

                        break;
                    }
                    case q: bScore += countBits(Attacks::getBishopAttacks(sq, board.occ[BOTH]) | Attacks::getRookAttacks(sq, board.occ[BOTH])) *
                            queenMobility;
                            break;
                    case k: {

                        // positional scores
                        bScore += kingScore[mirrorSquare[sq]];

                        // semi open file penalty
                        if ((board.bb[p] & fileMasks[sq]) == 0)
                                bScore -= semiOpenFileScore;

                        // open file penalty
                        if (((board.bb[P] | board.bb[p]) & fileMasks[sq]) == 0)
                                bScore -= openFileScore;

                        // king safety
                        bScore += countBits(kingAttacks[sq] & board.occ[BLACK]) * 5;

                        break;
                    }
                }
            }
        }

        // bishop pair scores
        if (countBits(board.bb[B]) == 2)
                wScore += bishopPairBonus;
        if (countBits(board.bb[b]) == 2)
                bScore += bishopPairBonus;

        return (board.side == WHITE) ? wScore - bScore : bScore - wScore;
    }
}