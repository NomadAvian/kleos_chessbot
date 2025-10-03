import sys
import argparse
import chess
import chess.engine
import chess.pgn
import random
import time

# TODO: add system arg parser to handle config

# -- CONFIG ---
ENGINE1_NAME = "kleos1"
ENGINE2_NAME = "kleos2"
ENGINE1_PATH = "./bin/kleos_classic"
ENGINE2_PATH = "./bin/kleos_classic"
# same opening every two games with alternating colors
NUM_GAMES    = 10
BASE_TIME    = 120.0 # seconds
INCREMENT    = 2.0 # seconds
# ---

# -- LOAD POSITIONS ---
def load_positions(filename = "./bench/positions.txt"):

    positions = []    

    with open(filename) as f:

        for line in f:
            line = line.strip()

            if not line:
                continue
            
            if ":" in line:
                name, fen = line.split(":", 1)
                positions.append((name.strip(), fen.strip()))
            else:
                positions.append(("Unknown", line))

    return positions
# ---

# --- PLAY A MATCH ---
def play_match(engine1,
               engine2,
               positions,
               num_games=10,
               base_time=60.0,
               increment=0.0):

    # track results
    results = { ENGINE1_NAME : 0, ENGINE2_NAME : 0, "draw" : 0 }
    # pgn container
    games = []

    for g in range(num_games):

        # choose a random opening
        if g % 2 == 0:
            opening, fen = random.choice(positions)
        
        # setup the board
        board = chess.Board(fen)

        # alternate colors
        if g % 2 == 0:
            white_engine, black_engine = engine1, engine2
            white_name, black_name = ENGINE1_NAME, ENGINE2_NAME
        else:
            black_engine, white_engine = engine1, engine2
            white_name, black_name = ENGINE2_NAME, ENGINE1_NAME
        
        clock = {white_name: base_time, black_name: base_time}

        # log opening used
        print(f"Game {str(g+1)}")
        print(f"selected opening: {opening}")
        print(f"start FEN: {board.fen}")

        # PGN game object
        game = chess.pgn.Game()
        game.headers["White"] = white_name
        game.headers["Black"] = black_name
        game.headers["Round"] = str(g+1)
        game.headers["FEN"] = fen

        node = game

        move_counter = 0

        # play the game
        while not board.is_game_over():

            # manage clock
            limit = chess.engine.Limit(
                white_clock = int(clock[white_name]),
                black_clock = int(clock[black_name]),
                white_inc  = int(increment),
                black_inc  = int(increment)
            )

            # make move
            engine = white_engine if board.turn == chess.WHITE else black_engine
            name = white_name if board.turn == chess.WHITE else black_name

            # start of think time
            move_start = time.time()
            # engine think
            result = engine.play(board, limit)
            move = result.move
            board.push(move)
            # total think time
            move_time = time.time() - move_start
            # subtract clock time
            clock[name] -= move_time

            move_counter += 1

            print(board)
            print(clock)

            # add move to PGN
            node = node.add_variation(move)

            # handle time control
            if clock[name] <= 0:
                print(f"Flag fall! {name} loses on time.")
                if name == white_name:
                    results[black_name] += 1
                    game.headers["Result"] = "0-1"
                else:
                    results[white_name] += 1
                    game.headers["Result"] = "1-0"
                break
            
            # add increment
            clock[name] += increment
        else:
            outcome = board.outcome()
            if outcome.winner is None:
                print("Draw!")
                results["draw"] += 1
                game.headers["Result"] = "1/2-1/2"
            elif outcome.winner == chess.WHITE:
                print(f"White ({white_name}) Wins!")
                results[white_name] += 1
                game.headers["Result"] = "1-0"
            else:
                print(f"Black ({black_name}) Wins!")
                results[black_name] += 1
                game.headers["Result"] = "0-1"
        
        # final position
        print(board)

        games.append(game)

    # write PGNs to a file
    with open("./bench/games.pgn", "w", encoding="utf-8") as f:
        for g in games:
            print(g, file=f, end="\n\n")
        
    print("Match Complete. Results:\n")
    print(results)
    return results
# ---

# --- MAIN ---
def main():

    # load positions
    positions = load_positions()

    # load engines
    engine1 = chess.engine.SimpleEngine.popen_uci(ENGINE1_PATH) 
    engine2 = chess.engine.SimpleEngine.popen_uci(ENGINE2_PATH)

    print(f"    \n\n--- {ENGINE1_NAME} vs {ENGINE2_NAME} ---\n")

    # play games
    play_match(engine1, engine2, positions)

    # exit the program
    sys.exit()
# ---

if __name__ == "__main__":
    main()