@echo off
if not exist ..\bin (
    mkdir ..\bin
)

cd ./src
g++ ^
main.cpp ^
bitboards.cpp ^
attacks.cpp ^
move.cpp ^
movegen.cpp ^
uci.cpp ^
perft.cpp ^
zobrist_hash.cpp ^
evaluation.cpp ^
search.cpp ^
hash_tables.cpp ^
-O3 -o ..\bin\kleos_classic.exe
cd ..\

echo ================================
echo Build finished successfully!
echo Executable: ..\bin\kleos_classic.exe
echo ================================
pause