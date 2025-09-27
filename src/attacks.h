#pragma once

#include "common.h"
#include "bitboards.h"

/*
    attack mask bitboards are used to quickly generate moves from a given position.

        1. leaper pieces (pawn, knight, king)
            move to target positions from their source squares.
            attack masks for leaper pieces can be calculated simply through offsets

        2. slider pieces (bishop, rook, queen)
            slide along a diagonal, file or rank.
            their movement varies depending on the board occupancy state.
            however, each square can have a limited number of relevant blocker bits.
            magic bitboards are used to hash blocker configurations for each square.

            the following magic numbers have been generated using xor-shift technique
        
        reference: https://www.chessprogramming.org/Magic_Bitboards
                   https://en.wikipedia.org/wiki/Xorshift
*/

extern U64 pawnAttacks[2][64];     // side, square
extern U64 knightAttacks[64];      // square
extern U64 kingAttacks[64];        // square
extern U64 bishopMasks[64];        // square
extern U64 bishopAttacks[64][512]; // square, magic number index
extern U64 rookMasks[64];          // square
extern U64 rookAttacks[64][4096];  // square, magic number index

constexpr U64 bishopMagicNumber[64] = {
    0x40040844404084ULL,   0x2004208a004208ULL,   0x10190041080202ULL,   0x108060845042010ULL,
    0x581104180800210ULL,  0x2112080446200010ULL, 0x1080820820060210ULL, 0x3c0808410220200ULL,
    0x4050404440404ULL,    0x21001420088ULL,      0x24d0080801082102ULL, 0x1020a0a020400ULL,
    0x40308200402ULL,      0x4011002100800ULL,    0x401484104104005ULL,  0x801010402020200ULL,
    0x400210c3880100ULL,   0x404022024108200ULL,  0x810018200204102ULL,  0x4002801a02003ULL,
    0x85040820080400ULL,   0x810102c808880400ULL, 0xe900410884800ULL,    0x8002020480840102ULL,
    0x220200865090201ULL,  0x2010100a02021202ULL, 0x152048408022401ULL,  0x20080002081110ULL,
    0x4001001021004000ULL, 0x800040400a011002ULL, 0xe4004081011002ULL,   0x1c004001012080ULL,
    0x8004200962a00220ULL, 0x8422100208500202ULL, 0x2000402200300c08ULL, 0x8646020080080080ULL,
    0x80020a0200100808ULL, 0x2010004880111000ULL, 0x623000a080011400ULL, 0x42008c0340209202ULL,
    0x209188240001000ULL,  0x400408a884001800ULL, 0x110400a6080400ULL,   0x1840060a44020800ULL,
    0x90080104000041ULL,   0x201011000808101ULL,  0x1a2208080504f080ULL, 0x8012020600211212ULL,
    0x500861011240000ULL,  0x180806108200800ULL,  0x4000020e01040044ULL, 0x300000261044000aULL,
    0x802241102020002ULL,  0x20906061210001ULL,   0x5a84841004010310ULL, 0x4010801011c04ULL,
    0xa010109502200ULL,    0x4a02012000ULL,       0x500201010098b028ULL, 0x8040002811040900ULL,
    0x28000010020204ULL,   0x6000020202d0240ULL,  0x8918844842082200ULL, 0x4010011029020020ULL
};

constexpr U64 rookMagicNumber[64] = {
    0x8a80104000800020ULL, 0x140002000100040ULL,  0x2801880a0017001ULL,  0x100081001000420ULL,
    0x200020010080420ULL,  0x3001c0002010008ULL,  0x8480008002000100ULL, 0x2080088004402900ULL,
    0x800098204000ULL,     0x2024401000200040ULL, 0x100802000801000ULL,  0x120800800801000ULL,
    0x208808088000400ULL,  0x2802200800400ULL,    0x2200800100020080ULL, 0x801000060821100ULL,
    0x80044006422000ULL,   0x100808020004000ULL,  0x12108a0010204200ULL, 0x140848010000802ULL,
    0x481828014002800ULL,  0x8094004002004100ULL, 0x4010040010010802ULL, 0x20008806104ULL,
    0x100400080208000ULL,  0x2040002120081000ULL, 0x21200680100081ULL,   0x20100080080080ULL,
    0x2000a00200410ULL,    0x20080800400ULL,      0x80088400100102ULL,   0x80004600042881ULL,
    0x4040008040800020ULL, 0x440003000200801ULL,  0x4200011004500ULL,    0x188020010100100ULL,
    0x14800401802800ULL,   0x2080040080800200ULL, 0x124080204001001ULL,  0x200046502000484ULL,
    0x480400080088020ULL,  0x1000422010034000ULL, 0x30200100110040ULL,   0x100021010009ULL,
    0x2002080100110004ULL, 0x202008004008002ULL,  0x20020004010100ULL,   0x2048440040820001ULL,
    0x101002200408200ULL,  0x40802000401080ULL,   0x4008142004410100ULL, 0x2060820c0120200ULL,
    0x1001004080100ULL,    0x20c020080040080ULL,  0x2935610830022400ULL, 0x44440041009200ULL,
    0x280001040802101ULL,  0x2100190040002085ULL, 0x80c0084100102001ULL, 0x4024081001000421ULL,
    0x20030a0244872ULL,    0x12001008414402ULL,   0x2006104900a0804ULL,  0x1004081002402ULL
};

constexpr int bishopRelevantBits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

constexpr int rookRelevantBits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

namespace Attacks {

/* LEAPER PIECES */

/// @brief generate pawn attacks from a square in the relevant direction
/// @param side 
/// @param sq
/// @return U64
U64 maskPawnAttacks(int side, int sq);

/// @brief generate knight attacks from a square
/// @param sq
/// @return U64
U64 maskKnightAttacks(int sq);

/// @brief generate king attacks from a square
/// @param sq
/// @return U64
U64 maskKingAttacks(int sq);

/* SLIDER PIECES */

/// @brief Generate attacks masks along the diagonals
/// @param sq
/// @return U64
U64 maskBishopAttacks(int sq);

/// @brief Generate attack masks along the diagonals until encountering a blocker
/// @param sq
/// @param block 
/// @return U64
U64 bishopAttacksOnTheFly(int sq, U64 block);

/// @brief Generate attacks masks along files and ranks
/// @param square 
/// @return U64
U64 maskRookAttacks(int sq);

/// @brief Generate attack masks along files & ranks until encountering a blocker
/// @param sq
/// @param block 
/// @return U64
U64 rookAttacksOnTheFly(int square, U64 block);

/// @brief set occupancies on a bitboard from an attack mask
/// @param index 
/// @param bits_in_mask 
/// @param attack_mask 
/// @return U64
U64 setOccupancy(int index, int bitsInMask, U64 mask);

/// @brief initialize all attack tables
void initializeAttacks();

/* Move Generation Utilities */

/// @brief use precomputed magic numbers and occupancy to get bishop attacks
/// @param sq
/// @param occ
/// @return U64
inline U64 getBishopAttacks(int sq, U64 occ) {
    occ &= bishopMasks[sq];
    occ *= bishopMagicNumber[sq];
    occ >>= (64 - bishopRelevantBits[sq]);
    return bishopAttacks[sq][occ];
}

/// @brief use precomputed magic numbers and occupancy to get rook attacks
/// @param sq
/// @param occ
/// @return U64
inline U64 getRookAttacks(int sq, U64 occ) {
    occ &= rookMasks[sq];
    occ *= rookMagicNumber[sq];
    occ >>= (64 - rookRelevantBits[sq]);
    return rookAttacks[sq][occ];
}

/// @brief check if a square is attacked by a side.
/// mainly used to detect checks in a position
/// @param sq 
/// @param side 
/// @return bool
inline bool isSquareAttacked(int sq, int side) {
    if (side == WHITE && (pawnAttacks[BLACK][sq] & board.bb[P]))           return 1; // pawn attacks
    if (side == BLACK && (pawnAttacks[WHITE][sq] & board.bb[p]))           return 1;
    if (knightAttacks[sq] & ((side == WHITE) ? board.bb[N] : board.bb[n])) return 1; // knight atacks
    if (kingAttacks[sq] & ((side == WHITE) ? board.bb[K] : board.bb[k]))   return 1; // king attacks
    if (getBishopAttacks(sq, board.occ[BOTH]) & ((side == WHITE) ?
        board.bb[B] | board.bb[Q] : board.bb[b] | board.bb[q]))            return 1; // bishop attacks
    if (getRookAttacks(sq, board.occ[BOTH]) & ((side == WHITE) ?
        board.bb[R] | board.bb[Q] : board.bb[r] | board.bb[q]))            return 1; // rook attacks
    return 0; // no attacks
}

}