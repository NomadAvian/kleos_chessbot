# Kleos Chess Bot

## NOTE

I couldn't make *makefile* work. So, to compile and run the code, use the `.bat` file for windows and `.sh` file for linux systems.

## Features

### General

    - UCI protocol compliant
    - Can play time controls

### Board Representation and Move Generation

    - bitboard based board representation
    - 0x88 move generation
    - precalculated attack tables
    - encoding moves in 32-bit integers
    - zobrist hashing
    - transposition tables

### Search

    - Negamax implementation of minimax algorithm
    - alpha-beta pruning
    - fail-hard beta cutoff
    - move ordering (MVV-LVA, PV)
    - null move pruning
    - late move reduction
    - principle variation search
    - killer moves & history moves
    - iterative deepening
    - quiescence search

### Evaluation

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
- [ ] add benchmark puzzles
- [ ] add benchmark endgames
- [ ] add documentation (files & comments)
- [ ] **add makefile**
- [ ] benchmark elo score
- [ ] open lichess bot account
- [ ] syzygy tablebase (?)
- [ ] add 50-move rule (?)
- [ ] **add nnue**