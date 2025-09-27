#pragma once

#include "common.h"
#include "bitboards.h"
#include "attacks.h"
#include "move.h"
#include "zobrist_hash.h"

/*
    in 0x88 board representation,
    moves in a position can be generated extremely fast using bit operations with
    pre-calculated attack masks.
*/

namespace MoveGen {

    /// @brief generate all psuedo legal moves in current position
    /// @param moveList 
    void generateMoves(MoveList &moveList);

}