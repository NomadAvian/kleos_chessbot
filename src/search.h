#pragma once

#include "common.h"
#include "bitboards.h"
#include "move.h"
#include "movegen.h"
#include "hash_tables.h"
#include "evaluation.h"

/* WARNING: CIRCULAR DEPENDENCY, SHOULD BE DECOUPLED LATER (?) */
#include "uci.h"

/*

*/

namespace Search {

    extern int quit;                     // exit from engine flag 
    extern int movestogo;                // UCI "movestogo" command counter
    extern int movetime;                 // UCI "movetime" command timer
    extern int time;                     // UCI "time" command holder (ms)
    extern int inc;                      // UCI "inc" command time increment holder
    extern int starttime;                // UCI "starttime" command time holder
    extern int stoptime;                 // UCI  "stoptime" comand time holder
    extern int timeset;                  // variable flag for time control availability
    extern int stopped;                  // variable flag fot time up
    
    constexpr int maxPly           = 64; // max ply during search
    constexpr int aspirationWindow = 50; // aspiration window adjust constant
    constexpr int fullDepthMoves   = 4;  // for Late Move reduction (LMR)
    constexpr int R                = 3;  // for Null Move Pruning (NMP)

    extern unsigned long long nodes;     // total searched nodes (positions) during a search

    extern const int MVVLVA[12][12];     // most valuable victim, least valuable attacker score table [attacker][victim]

    extern int killerMoves[2][64];       // killer moves [id][ply]
    extern int historyMoves[12][64];     // history moves [piece][square]

    extern int pvTable[maxPly][maxPly];  // principle variation tables
    extern int pvLength[maxPly];         // principle variation lengths

    extern int followPV, scorePV;        // follow PV & PV scores

    /// @brief perform a search from the current position
    /// @param depth 
    int searchPosition(int depth);
}