#include "hash_tables.h"

const int hfExact  = 0;
const int hfAlpha  = 1;
const int hfBeta   = 2;

const int noHash   = 100000;

TT::HashTable TT::tt[hashSize];

void TT::clearTT() {
    for (HashTable &ttable : tt) {
        ttable.hashKey = 0ULL;
        ttable.depth   = 0;
        ttable.flag    = 0;
        ttable.score   = 0;
    }
}