#pragma once

#include "common.h"
#include "bitboards.h"

/*
    zobrist hashes attempt to transform a board position into a number of set length
    with an equal distribution over all possible numbers

    in this implementation, each component of a position gets a randomly generated U64 numuber as a hash key.
    this random number is generated using xor-shift.
    the hash keys are sequentially xor'ed to get the hash key for the overall position.
    the final hash of the position is a U64 number. as a consequence, there is likelihood of collisions.

    hashes are used to detect 3 fold repetitions & to speed up search by skipping previously
    evaluated positions.

    references: https://research.cs.wisc.edu/techreports/1970/TR88.pdf
*/

namespace Zobrist {

extern U64 pieceKeys[12][64]; // random pieces keys [piece][square]
extern U64 enpassantKeys[64]; // random en passant keys [square]
extern U64 castleKeys[16];    // random castle keys [castling rights code]
extern U64 sideKey;           // random side key

/// @brief initialize random hash keys for all components
void initializeRandomKeys();

/// @brief generate a hash key from current position
/// @return U64 hash key
U64 generateHashKey();

}