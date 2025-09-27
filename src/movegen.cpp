#include "movegen.h"

inline void generatePawnMoves(MoveList &moveList, int side) {
    int from, to;
    U64 attacks;
    int piece         = (side == WHITE) ?  P : p;
    U64 pieceBB       = board.bb[piece];
    int forward       = (side == WHITE) ? -8 : 8;
    int startRank     = (side == WHITE) ?  6 : 1; // rank 2/7
    int promotionRank = (side == WHITE) ?  1 : 6; // rank 7/2

    while (pieceBB) {
        from = popLS1B(pieceBB);
        to   = from + forward;
        if (to >= a8 && to <= h1 && getBit(board.occ[BOTH], to) == 0) {
            // promotions
            if ((from >> 3) == promotionRank) {
                addMove(moveList, encodeMove(from, to, piece, (side == WHITE) ? Q: q, 0, 0, 0, 0));
                addMove(moveList, encodeMove(from, to, piece, (side == WHITE) ? R: r, 0, 0, 0, 0));
                addMove(moveList, encodeMove(from, to, piece, (side == WHITE) ? B: b, 0, 0, 0, 0));
                addMove(moveList, encodeMove(from, to, piece, (side == WHITE) ? N: n, 0, 0, 0, 0));
            } else {
                // One square forward
                addMove(moveList, encodeMove(from, to, piece, blank, 0, 0, 0, 0));
                // Two-square push
                if ((from >> 3) == startRank) {
                    int doublePush = to + forward;
                    if (getBit(board.occ[BOTH], doublePush) == 0)
                        addMove(moveList, encodeMove(from, doublePush, piece, blank, 0, 1, 0, 0));
                }
            }
        }
        // Pawn captures
        attacks = pawnAttacks[side][from] & board.occ[side ^ 1];
        while (attacks) {
            to = popLS1B(attacks);
            if ((from >> 3) == promotionRank) {
                addMove(moveList, encodeMove(from, to, piece, (side == WHITE ? Q: q), 1, 0, 0, 0));
                addMove(moveList, encodeMove(from, to, piece, (side == WHITE ? R: r), 1, 0, 0, 0));
                addMove(moveList, encodeMove(from, to, piece, (side == WHITE ? B: b), 1, 0, 0, 0));
                addMove(moveList, encodeMove(from, to, piece, (side == WHITE ? N: n), 1, 0, 0, 0));
            }
            else
                addMove(moveList, encodeMove(from, to, piece, blank, 1, 0, 0, 0));
            
        }
        // En passant
        if (board.enpass != no_sq)
            if (pawnAttacks[side][from] & (1ULL << board.enpass))
                addMove(moveList, encodeMove(from, board.enpass, piece, blank, 1, 0, 1, 0));
    }
}

inline void generateCastles(MoveList &moveList, int side) {
    U64 piece_bitboard = board.bb[((side == WHITE) ? K : k)];
    if (side == WHITE) {
        if (board.castle & wk) {
            if (!getBit(board.occ[BOTH], f1) && !getBit(board.occ[BOTH], g1) &&
                !Attacks::isSquareAttacked(e1, BLACK)  && !Attacks::isSquareAttacked(f1, BLACK))
                addMove(moveList, encodeMove(e1, g1, K, blank, 0, 0, 0, 1));
            
        }
        if (board.castle & wq) {
            if (!getBit(board.occ[BOTH], d1) && !getBit(board.occ[BOTH], c1) && !getBit(board.occ[BOTH], b1) &&
                !Attacks::isSquareAttacked(e1, BLACK)  && !Attacks::isSquareAttacked(d1, BLACK))
                addMove(moveList, encodeMove(e1, c1, K, blank, 0, 0, 0, 1));
        }
    } else {
        if (board.castle & bk) {
            if (!getBit(board.occ[BOTH], f8) && !getBit(board.occ[BOTH], g8) &&
                !Attacks::isSquareAttacked(e8, WHITE)  && !Attacks::isSquareAttacked(f8, WHITE))
                addMove(moveList, encodeMove(e8, g8, k, blank, 0, 0, 0, 1));
        }
        if (board.castle & bq) {
            if (!getBit(board.occ[BOTH], d8) && !getBit(board.occ[BOTH], c8) && !getBit(board.occ[BOTH], b8) &&
                !Attacks::isSquareAttacked(e8, WHITE)  && !Attacks::isSquareAttacked(d8, WHITE))
                addMove(moveList, encodeMove(e8, c8, k, blank, 0, 0, 0, 1));
        }
    }
}

inline void generateKnightMoves(MoveList &moveList, int side) {
    U64 pieceBB = (side == WHITE ? board.bb[N] : board.bb[n]);
    int from, to;
    while (pieceBB) {
        from = popLS1B(pieceBB);
        U64 attacks = knightAttacks[from] & ~board.occ[side];
        while (attacks) {
            to = popLS1B(attacks);
            if (getBit(board.occ[side ^ 1], to))
                addMove(moveList, encodeMove(from, to, (side == WHITE ? N : n), blank, 1, 0, 0, 0));
            else
                addMove(moveList, encodeMove(from, to, (side == WHITE ? N : n), blank, 0, 0, 0, 0));
        }
    }
}

inline void generateBishopMoves(MoveList &moveList, int side) {
    U64 pieceBB = (side == WHITE ? board.bb[B] : board.bb[b]);
    int from, to;
    while (pieceBB) {
        from = popLS1B(pieceBB);
        U64 attacks = Attacks::getBishopAttacks(from, board.occ[BOTH]) & ~board.occ[side];
        while (attacks) {
            to = popLS1B(attacks);
            if (getBit(board.occ[side ^ 1], to))
                addMove(moveList, encodeMove(from, to, side == WHITE ? B : b, blank, 1, 0, 0, 0));
            else
                addMove(moveList, encodeMove(from, to, side == WHITE ? B : b, blank, 0, 0, 0, 0));
        }
    }
}

inline void generateRookMoves(MoveList &moveList, int side) {
    U64 pieceBB = (side == WHITE ? board.bb[R] : board.bb[r]);
    int from, to;
    while (pieceBB) {
        from = popLS1B(pieceBB);
        U64 attacks = Attacks::getRookAttacks(from, board.occ[BOTH]) & ~board.occ[side];
        while (attacks) {
            to = popLS1B(attacks);
            if (getBit(board.occ[side ^ 1], to))
                addMove(moveList, encodeMove(from, to, side == WHITE ? R : r, blank, 1, 0, 0, 0));
            else
                addMove(moveList, encodeMove(from, to, side == WHITE ? R : r, blank, 0, 0, 0, 0));
        }
    }
}

inline void generateKingMoves(MoveList &moveList, int side) {
    U64 pieceBB = (side == WHITE ? board.bb[K] : board.bb[k]);
    int from, to;
    while (pieceBB) {
        from = popLS1B(pieceBB);
        U64 attacks = kingAttacks[from] & ~board.occ[side];
        while (attacks) {
            to = popLS1B(attacks);
            if (getBit(board.occ[side ^ 1], to))
                addMove(moveList, encodeMove(from, to, side == WHITE ? K : k, blank, 1, 0, 0, 0));
            else
                addMove(moveList, encodeMove(from, to, side == WHITE ? K : k, blank, 0, 0, 0, 0));
        }
    }
}

inline void generateQueenMoves(MoveList &moveList, int side) {
    U64 pieceBB = (side == WHITE ? board.bb[Q] : board.bb[q]);
    int from, to;
    while (pieceBB) {
        from = popLS1B(pieceBB);
        U64 attacks = (Attacks::getBishopAttacks(from, board.occ[BOTH]) | Attacks::getRookAttacks(from, board.occ[BOTH]))
                      & ~board.occ[side];
        while (attacks) {
            to = popLS1B(attacks);
            if (getBit(board.occ[side ^ 1], to))
                addMove(moveList, encodeMove(from, to, side == WHITE ? Q : q, blank, 1, 0, 0, 0));
            else
                addMove(moveList, encodeMove(from, to, side == WHITE ? Q : q, blank, 0, 0, 0, 0));
        }
    }
}

void MoveGen::generateMoves(MoveList &moveList) {
    generatePawnMoves  (moveList, board.side);
    generateCastles    (moveList, board.side);
    generateKingMoves  (moveList, board.side);
    generateKnightMoves(moveList, board.side);
    generateBishopMoves(moveList, board.side);
    generateRookMoves  (moveList, board.side);
    generateQueenMoves (moveList, board.side);
}