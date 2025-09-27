#include "bitboards.h"

/* GLOBAL BITBOARDS & BOARD STATE VARIABLES */

Board board;
int ply;
U64 repetitionTable[1000];
int repetitionIndex;

/* FUNCTIONS */

void reset() {
    // reset bitboards
    memset(board.bb, 0ULL, sizeof(board.bb));
    memset(board.occ, 0ULL, sizeof(board.occ));
    // reset additional variables
    board.side = 0;
    board.castle = 0;
    board.enpass = no_sq;
    board.hashKey = 0ULL;
    // reset repetition tables
    repetitionIndex = 0;
    memset(repetitionTable, 0ULL, sizeof(repetitionTable));
}

void printBitboard(U64 bb) {
    printf("\n");
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++) 
        {
            int sq = (rank << 3) + file;
            if (getBit(bb, sq))
                printf(" 1 ");
            else
                printf(" 0 ");
        }
        printf("\n");
    }
    // printf("\n\n    Integer: %llx\n", bb);
}

void printBoard() {
    printf("\n");
    for (int rank = 0; rank < 8; rank++)
    {
        for (int file = 0; file < 8; file++) 
        {
            int square = (rank<<3) + file;
            if (!file)
                printf("  %d ", 8 - rank);
            int piece = blank;
            for (int i = 0; i < 12; i++)
            {
                if (getBit(board.bb[i], square))
                {
                    piece = i;
                    break;
                }
            }
            printf(" %c ", pieceToAscii[piece]);
        }
        printf("\n");
    }
    printf("     a  b  c  d  e  f  g  h\n\n");
    printf("     Side to move : %s\n", board.side == WHITE ? "white" : "black");
    printf("     Enpassant    : %s\n", (board.enpass != no_sq ? sqToCoords[board.enpass] : "none"));
    printf("     Castling     : %c%c%c%c\n\n", (board.castle & wk) ? 'K' : '-',
                                               (board.castle & wq) ? 'Q' : '-',
                                               (board.castle & bk) ? 'k' : '-',
                                               (board.castle & bq) ? 'q' : '-');

    // printf("     Hash Key     : %llx\n\n", board.hashKey);
}