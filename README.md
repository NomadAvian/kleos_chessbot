# Kleos Chess Bot

## Getting Started

Kleos is a UCI compliant chess bot. To make it play games, compile the binaries. You can then load it into a chess GUI like [Arena](http://www.playwitharena.de/) or [CuteChess](https://cutechess.com/).

## Compilation

You will need to have [makefile](https://makefiletutorial.com/) and **g++** compiler to do the following. Create a folder `bin` in the working directory.

```bash
# compile the binaries
$ make build
# run the application
$ make run
# clean the binaries
$ make clean
# do all of the above sequentially
$ make
```

> Currently, it is relatively weak. Expect a much stronger engine soon!

## Features

### General

- UCI protocol compliant
- plays time controls

### Board Representation and Move Generation

- bitboard based board representation
- 0x88 move generation
- precalculated attack tables
- encoding moves in 32-bit integers
- zobrist hashing
- transposition tables

### Search

- negamax implementation of minimax algorithm
- alpha-beta pruning
- fail-hard beta cutoff
- move ordering (MVV-LVA, PV)
- null move pruning
- late move reduction
- principle variation search
- killer moves & history moves
- iterative deepening
- quiescence search

### Evaluation (Hand-Crafted Heuristics)

- material count
- positional bonuses
- pawn structure bonuses/penalties (doubled, isolated, passed pawns)
- open/semi-open file bonuses/penalties (rooks, kings)
- king safety
- piece mobility scores
- bishop pair bonus

# TO-DO

- [x] add time control features (UCI)
    - [ ] understand & refactor time management code
    - [ ] add advanced time management strategy
- [ ] improve static evaluation
    - [ ] add game phase aware evaluation (tapered eval)
    - [ ] add more robust positional score tables
- [ ] improve search
    - [ ] attenuate pruning to not overlook prophylaxis
    - [ ] consider checks & promotions in quiescence search
    - [ ] experiment with fail-soft beta cutoff
- [x] add python script for running test games from set openings
- [ ] add benchmark puzzles
- [ ] add benchmark endgames
- [ ] add documentation (files & comments)
- [x] **add makefile**
- [ ] benchmark elo score
- [ ] open lichess bot account
- [ ] syzygy tablebase (?)
- [ ] add 50-move rule (?)
- [ ] **add nnue**