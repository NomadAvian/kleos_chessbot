#include "perft.h"

unsigned long long perftNodes;

/// @brief brute force traversal of the game tree to count positions reached
/// @param depth 
inline void perftDriver(int depth) {
    // depth reached
    if (depth == 0) {
        perftNodes++;
        return;
    }
    MoveList moveList;
    MoveGen::generateMoves(moveList);
    for(int i = 0; i < moveList.count; i++) {
        Board temp_board = board;
        if (makeMove(moveList.moves[i], 0) == 0)
            continue;
        perftDriver(depth-1);
        board = temp_board;
    }
}

void Perft::perftTest(const char *fen, int depth) {
    UCI::parseFEN(fen);
    printf("\n--- Performance Test ---\n");
    perftNodes = 0;
    clock_t startTime = clock();
    MoveList moveList;
    MoveGen::generateMoves(moveList);
    
    // debug print
    // for (int i = 0; i < moveList.count; i++) {
    //     printf("No. %d ", i+1);
    //     printMove(moveList.moves[i]);
    //     printf("\n");
    // }

    for (int i = 0; i < moveList.count; i++) {
        int move = moveList.moves[i];
        Board temp_board = board;
        if (makeMove(move, 0) == 0)
            continue;
        long long cumulativeNodes = perftNodes;
        perftDriver(depth-1);
        board = temp_board;

        long long oldNodes = perftNodes - cumulativeNodes;
        printf("move: %s%s%-1s, nodes: %llu\n", sqToCoords[getFromSquare(move)],
                                                sqToCoords[getToSquare(move)],
                                                promotedPieces[getPromotedPiece(move)],
                                                oldNodes);
    }
    double elapsedTime = 1000.0 * (double) (clock() - startTime) / CLOCKS_PER_SEC;
    printf("\n    depth: %d", depth);
    printf("\n    nodes: %lld", perftNodes);
    printf("\n    runtime: %.6f ms\n\n", elapsedTime);
}