#pragma once

#include <chrono>

#include "common.h"
#include "move.h"
#include "movegen.h"
#include "uci.h"

/*
    perft tests (performance tests) are done to benchmark the accuracy and speed of a move generator
*/

extern unsigned long long perftNodes; // total positions reached during perft test

namespace Perft {

    /// @brief do a perft test from a given position to a given depth
    /// @param fen 
    /// @param depth 
    void perftTest(const char *fen, int depth);

}