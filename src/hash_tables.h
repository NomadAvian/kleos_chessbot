#pragma once

#include "common.h"
#include "bitboards.h"
#include "zobrist_hash.h"
#include "evaluation.h"

/*
    transposition tables store hash keys of previously visited positions to avoid searching the same game tree twice.
    for brevity, in this code base it is refered to as 'hash tables'
*/

// hash table flags
extern const int hfExact;             // position has exact evaluation score flag
extern const int hfAlpha;             // position fails low flag
extern const int hfBeta;              // position fails high flag

constexpr    int hashSize = 800000;   // hash table size (20MB)
extern const int noHash;              // no hash entry exists (value outside alpha-beta bounds)

namespace TT {

    /// @brief hash table data structure
    struct HashTable {
        U64 hashKey;   // almost unique hash key
        int depth;     // current search depth
        int flag;      // type of node: PV / fail low / fail high
        int score;     // PV / alpha / beta

        // some implementations contain best moves (?)
    };

    extern HashTable tt[hashSize]; // main hash (transposition) table for search

    /// @brief clear transposition data from the main hash table
    void clearTT();

    /// @brief read hash entry data from the main hash table
    /// @param alpha 
    /// @param beta 
    /// @param depth 
    /// @return 
    inline int readHashEntry(int alpha, int beta, int depth) {
        HashTable *hashEntry = &tt[board.hashKey % hashSize];
        // verify hash values match
        if (hashEntry->hashKey == board.hashKey)
            // verify depths match
            if (hashEntry->depth >= depth) {
                int score = hashEntry->score;
                // adjust distance from current position
                if (score < -mateScore)
                    score += ply;
                if (score > mateScore)
                    score -= ply;
                // PV node
                if (hashEntry->flag == hfExact)
                    return score;
                // fail low
                if (hashEntry->flag == hfAlpha && score <= alpha)
                    return alpha;
                // fail high
                if (hashEntry->flag == hfBeta && score >= beta)
                    return beta;
            }
        return noHash;
    }

    /// @brief write hash entry data to main hash table
    /// @param score 
    /// @param depth 
    /// @param hash_flag 
    inline void writeHashEntry(int score, int depth, int hashFlag) {
        HashTable *hashEntry = &tt[board.hashKey % hashSize];
        // adjust distance from current position
        if (score < -mateScore)
            score -= ply;
        if (score > mateScore)
            score += ply;
        hashEntry->hashKey = board.hashKey;
        hashEntry->score   = score;
        hashEntry->depth   = depth;
        hashEntry->flag    = hashFlag;
    }
}