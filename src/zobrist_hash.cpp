#include "zobrist_hash.h"

U64 Zobrist::pieceKeys[12][64];
U64 Zobrist::enpassantKeys[64];
U64 Zobrist::castleKeys[16];
U64 Zobrist::sideKey;

/* XOR shift random number generation */

unsigned int randomState = 1804289383;

unsigned int getRandomU32Number() {
    unsigned int number = randomState;
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;
    randomState = number;
    return randomState;
}

U64 getRandomU64Number() {
    U64 n1, n2, n3, n4;
    n1 = (U64) (getRandomU32Number()) & 0xFFFF;
    n2 = (U64) (getRandomU32Number()) & 0xFFFF;
    n3 = (U64) (getRandomU32Number()) & 0xFFFF;
    n4 = (U64) (getRandomU32Number()) & 0xFFFF;
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

/* INITIALIZATIONS */

void Zobrist::initializeRandomKeys() {
    // piece keys
    for (int piece = P; piece <= k; piece++)
        for (int square = 0; square < 64; square++)
            pieceKeys[piece][square] = getRandomU64Number();
    // en passant keys
    for (int square = 0; square < 64; square++)
        enpassantKeys[square] = getRandomU64Number();
    // castling keys
    for (int index = 0; index < 16; index++)
        castleKeys[index] = getRandomU64Number();
    // side key
    sideKey = getRandomU64Number();
}

U64 Zobrist::generateHashKey() {
    U64 hashKey = 0ULL;
    U64 bb;
    // piece keys
    for (int piece = P; piece <= k; piece++) {
        bb = board.bb[piece];
        while(bb)
            hashKey ^= pieceKeys[piece][popLS1B(bb)];
    }
    // en passant key
    if (board.enpass != no_sq) hashKey ^= enpassantKeys[board.enpass];
    // caslte key
    hashKey ^= castleKeys[board.castle];
    // side key
    if (board.side == BLACK) hashKey ^= sideKey;
    return hashKey;
}