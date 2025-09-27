#include "move.h"

void printMove(int move) {
    printf("%s%s%s", sqToCoords[getFromSquare(move)],
                     sqToCoords[getToSquare(move)],
                     promotedPieces[getPromotedPiece(move)]);
}

void printMovelist(MoveList moveList) {
    if (!moveList.count)
    {
        printf("No moves in move list");
        return;
    }

    printf("\n%-4s %-4s  %-6s %-7s %-7s %-10s %-9s\n",
       "No.", "Move", "Piece", "Capture", "Double", "EnPassant", "Castling");
    for (int move_count = 0; move_count < moveList.count; move_count++)
    {
        // int move = moveList.moves[move_count];
        // int source_square = getFromSquare(move);
        // int target_square = getToSquare(move);
        // int piece = getPiece(move);
        // int promoted_piece = getPromotedPiece(move);
        // bool capture = isCapture(move);
        // bool double_push = isDoublePush(move);
        // bool en_passant = isEnpass(move);
        // bool castling = isCastle(move);

        // // Construct move string: e.g., e2e4 or e7e8q if promotion
        // char move_str[6];
        // snprintf(move_str, sizeof(move_str), "%s%s",
        //         sqToCoords[source_square],
        //         sqToCoords[target_square]);

        // printf("%-4d %-4s%s  %-6s %-6s %-7s %-10s %-9s\n",
        //     move_count + 1,
        //     move_str,
        //     (promotedPieces[promoted_piece] != "") ? promotedPieces[promoted_piece] : " ",
        //     piece_to_ascii[piece],
        //     capture ? "1" : "0",
        //     double_push ? "1" : "0",
        //     en_passant ? "1" : "0",
        //     castling ? "1" : "0");

        printf("No. %d ", move_count+1);
        printMove(moveList.moves[move_count]);
        printf("\n");
    }
}