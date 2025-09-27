#pragma once

#include <stdlib.h>
#include <unordered_map>

#include "common.h"
#include "bitboards.h"
#include "move.h"
#include "movegen.h"
#include "zobrist_hash.h"
#include "search.h"

/*
    UCI (universal chess interface) is a communication protocol that allows chess engines to interact with GUIs
*/

namespace UCI {

    /// @brief listen for input to interrupt search
    void communicate();

    /// @brief parse an fen and set up the main board accordingly
    /// @param fen 
    void parseFEN(const char *fen);

    /// @brief main UCI loop
    void uciLoop();
}