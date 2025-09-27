#include "common.h"
#include "uci.h"
#include "perft.h"
#include "evaluation.h"

const char* trickyFEN   = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - ";
const char* doubleFEN   = "8/p7/8/8/P7/P7/8/8 w - - 0 1";
const char* isolatedFEN = "8/p5p1/8/8/P7/P5P1/8/8 w - - 0 1";
const char* passedFEN   = "8/p7/Pp1p4/1P6/8/8/8/8 w - - 0 1";

int main() {

    Attacks::initializeAttacks();
    Zobrist::initializeRandomKeys();
    Evaluation::initializeEvaluationMasks();
    TT::clearTT();
    
    int debug = 0;

    if (debug) {
        // UCI::parseFEN("3q4/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/3Q4 w - - 0 1");
        // UCI::parseFEN(trickyFEN);
        // printBoard();
        // printf("    Evaluation: %d\n", Evaluation::evaluate());
        // Search::searchPosition(10);
    } else {
        UCI::uciLoop();
    }

    return 0;
}