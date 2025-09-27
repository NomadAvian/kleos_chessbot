#!/bin/bash

mkdir -p ../bin
cd src || exit

g++ -O3 \
    main.cpp \
    bitboards.cpp \
    attacks.cpp \
    move.cpp \
    movegen.cpp \
    uci.cpp \
    perft.cpp \
    zobrist_hash.cpp \
    evaluation.cpp \
    search.cpp \
    hash_tables.cpp \
    -o ../bin/kleos_classic.exe

cd ..

echo "Build finished. Executable: ../bin/kleos_classic.exe"