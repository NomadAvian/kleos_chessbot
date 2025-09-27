#include "attacks.h"

U64 pawnAttacks[2][64];     // side, square
U64 knightAttacks[64];      // square
U64 kingAttacks[64];        // square
U64 bishopMasks[64];        // square
U64 bishopAttacks[64][512]; // square, magic number index
U64 rookMasks[64];          // square
U64 rookAttacks[64][4096];  // square, magic number index

/*  LEAPER PIECES */

static constexpr U64 notAFile  = 18374403900871474942ULL; // mask non a-file squares
static constexpr U64 notHFile  = 9187201950435737471ULL;  // mask non h-file squares
static constexpr U64 notABFile = 18229723555195321596ULL; // mask non a,b-file squares
static constexpr U64 notHGFile = 4557430888798830399ULL;  // mask non h,g-file squares

U64 Attacks::maskPawnAttacks(int side, int sq) {
    U64 bb = 0ULL;
    setBit(bb, sq);
    if (side == WHITE)
        return ((bb >> 7) & notAFile) | ((bb >> 9) & notHFile);
    else
        return ((bb << 9) & notAFile) | ((bb << 7) & notHFile);
}

U64 Attacks::maskKnightAttacks(int sq) {
    U64 bb = 0ULL;
    setBit(bb, sq);
    return ((bb >> 17) & notHFile ) | ((bb >> 15) & notAFile)  |
           ((bb >> 10) & notHGFile) | ((bb >> 6 ) & notABFile) |
           ((bb << 17) & notAFile ) | ((bb << 15) & notHFile)  |
           ((bb << 10) & notABFile) | ((bb << 6 ) & notHGFile);
}

U64 Attacks::maskKingAttacks(int sq)
{
    U64 bb = 0ULL;
    setBit(bb, sq);
    return ((bb >> 1) & notHFile) | ((bb >> 9) & notHFile) | ((bb << 7) & notHFile) |
           ((bb << 1) & notAFile) | ((bb << 9) & notAFile) | ((bb >> 7) & notAFile) |
           ((bb << 8) | (bb >> 8));
}

/* SLIDER PIECES */

// bishop move offsets
static constexpr int bishopDirections[4][2] = { 
    {1,1}, {1,-1}, {-1,1}, {-1,-1}
};

// rook move offsets
static constexpr int rookDirections[4][2]   = { 
    {1,0}, {-1,0}, {0,1}, {0,-1}
};

U64 Attacks::maskBishopAttacks(int sq) {
    U64 bb = 0ULL;
    int r = sq >> 3, f = sq & 7;
    int tr, tf;
    for (auto [dx, dy] : bishopDirections)
        for (tr = r + dx, tf = f + dy; tr >= 1 && tr <= 6 && tf >= 1 && tf <= 6; tr += dx, tf += dy)
            bb |= 1ULL << ((tr << 3) + tf);
    return bb;
}

U64 Attacks::bishopAttacksOnTheFly(int sq, U64 block) {
    U64 bb = 0ULL;
    int r = sq >> 3, f = sq & 7;
    int tr, tf;
    for (auto [dx, dy] : bishopDirections)
        for (tr = r + dx, tf = f + dy; tr >= 0 && tr <= 7 && tf >= 0 && tf <= 7; tr += dx, tf += dy) {
            U64 sqBit = 1ULL << ((tr << 3) + tf);
            bb |= sqBit;
            if (sqBit & block)
                break;
        }
    return bb;
}

U64 Attacks::maskRookAttacks(int sq) {
    U64 bb = 0ULL;
    int r = sq >> 3, f = sq & 7;
    int tr, tf;
    for (tr = r + 1; tr <= 6; tr++)
        bb |= (1ULL<<((tr<<3) + f));
    for (tr = r - 1; tr >= 1; tr--)
        bb |= (1ULL<<((tr<<3) + f));
    for (tf = f + 1; tf <= 6; tf++)
        bb |= (1ULL<<((r<<3) + tf));
    for (tf = f - 1; tf >= 1; tf--)
        bb |= (1ULL<<((r<<3) + tf));
    return bb;
}

U64 Attacks::rookAttacksOnTheFly(int sq, U64 block) {
    U64 bb = 0ULL;
    int r = sq >> 3, f = sq & 7;
    int tr, tf;
    for (auto [dx, dy] : rookDirections)
        for (tr = r + dx, tf = f + dy; tr >= 0 && tr <= 7 && tf >= 0 && tf <= 7; tr += dx, tf += dy) {
            U64 sqBit = 1ULL << ((tr << 3) + tf);
            bb |= sqBit;
            if (sqBit & block)
                break;
        }
    return bb;
}

U64 Attacks::setOccupancy(int index, int bitsInMask, U64 mask) {
    U64 occupancy = 0ULL;
    for (int count = 0; count < bitsInMask; count++) {
        int sq = popLS1B(mask);
        if (index & (1 << count))
            occupancy |= (1ULL << sq);
    }
    return occupancy;
}

/* INITIALIZATION */

void Attacks::initializeAttacks() {
    for(int sq = 0; sq < 64; sq++) {
        // leaper pieces
        pawnAttacks[WHITE][sq] = Attacks::maskPawnAttacks(WHITE, sq);
        pawnAttacks[BLACK][sq] = Attacks::maskPawnAttacks(BLACK, sq);
        knightAttacks[sq]      = Attacks::maskKnightAttacks(sq);
        kingAttacks[sq]        = Attacks::maskKingAttacks(sq);

        // slider pieces
        for (int bishop = 0; bishop <= 1; bishop++) {
            bishopMasks[sq] = Attacks::maskBishopAttacks(sq);
            rookMasks[sq]   = Attacks::maskRookAttacks(sq);
            U64 attackMask = bishop ? bishopMasks[sq] : rookMasks[sq];
            int relevantBits = countBits(attackMask);
            int occupancyIndices = (1 << relevantBits);
            for (int index = 0; index < occupancyIndices; index++) {
                U64 occupancy = setOccupancy(index, relevantBits, attackMask);
                if (bishop) {
                    int magicIndex = (int) ((occupancy * bishopMagicNumber[sq]) >> (64 - bishopRelevantBits[sq]));
                    bishopAttacks[sq][magicIndex] = bishopAttacksOnTheFly(sq, occupancy);
                } else {
                    int magicIndex = (int) ((occupancy * rookMagicNumber[sq]) >> (64 - rookRelevantBits[sq]));
                    rookAttacks[sq][magicIndex] = rookAttacksOnTheFly(sq, occupancy);
                }
            }
        }    
    }
}