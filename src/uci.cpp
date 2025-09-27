#include "uci.h"

std::unordered_map<char, int> piecesChar = {
    {'P', P}, {'N', N}, {'B', B}, {'R', R}, {'Q', Q}, {'K', K},
    {'p', p}, {'n', n}, {'b', b}, {'r', r}, {'q', q}, {'k', k},
    {'.', blank}
};

/* TIME CONTROL : FORKED FROM VICE ENGINE */

int input_waiting()
{
    #ifndef _WIN32
        fd_set readfds;
        struct timeval tv;
        FD_ZERO (&readfds);
        FD_SET (fileno(stdin), &readfds);
        tv.tv_sec=0; tv.tv_usec=0;
        select(16, &readfds, 0, 0, &tv);

        return (FD_ISSET(fileno(stdin), &readfds));
    #else
        static int init = 0, pipe;
        static HANDLE inh;
        DWORD dw;

        if (!init) {
            init = 1;
            inh = GetStdHandle(STD_INPUT_HANDLE);
            pipe = !GetConsoleMode(inh, &dw);
            if (!pipe)
            {
                SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
                FlushConsoleInputBuffer(inh);
            }
        }
        if (pipe) {
           if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
           return dw;
        } else {
           GetNumberOfConsoleInputEvents(inh, &dw);
           return dw <= 1 ? 0 : dw;
        }
    #endif
}

// read GUI/user input
void read_input()
{
    // bytes to read holder
    int bytes;
    
    // GUI/user input
    char input[256] = "", *endc;

    // "listen" to STDIN
    if (input_waiting())
    {
        // tell engine to stop calculating
        Search::stopped = 1;
        
        // loop to read bytes from STDIN
        do {
            // read bytes from STDIN
            #ifndef _WIN32
                int bytes = read(fileno(stdin), input, 255);
                if (bytes < 0) return;
                input[bytes] = '\0';
            #else
                DWORD bytes = 0;
                ReadFile(GetStdHandle(STD_INPUT_HANDLE), input, 255, &bytes, NULL);
                input[bytes] = '\0';
            #endif
        } while (bytes < 0); // until bytes available
        // searches for the first occurrence of '\n'
        endc = strchr(input,'\n');
        // if found new line set value at pointer to 0
        if (endc) *endc=0;
        
        // if input is available
        if (strlen(input) > 0) {
            // match UCI "quit" command
            if (!strncmp(input, "quit", 4)) {
                // tell engine to terminate exacution    
                Search::quit = 1;
            }
            // // match UCI "stop" command
            else if (!strncmp(input, "stop", 4)) {
                // tell engine to terminate exacution
                Search::quit = 1;
            }
        }   
    }
}

// a bridge function to interact between search and GUI input
void UCI::communicate() {
	// if time is up break here
    if(Search::timeset == 1 && getTimeMs() > Search::stoptime) {
		Search::stopped = 1;
	}
    // read GUI input
	read_input();
}

/* END OF FORKED CODE */

void UCI::parseFEN(const char *fen) {
    reset();
    TT::clearTT();
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 8; file++) {
            int square = (rank<<3) + file;
            // match ascii pieces with fen pieces
            if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
                int piece = piecesChar[*fen];
                setBit(board.bb[piece], square);
                fen++;
            }
            // empty squares 
            if (*fen >= '0' && *fen <= '9') {
                int offset = *fen - '0';
                file += offset;
                int piece = -1;
                for (int bb_piece = P; bb_piece <= k; bb_piece++) {
                    if (getBit(board.bb[bb_piece], square))
                        piece = bb_piece;
                }
                if (piece == -1)
                    file--;
                fen++;
            }
            // rank separators
            if (*fen == '/') {
                fen++;
                break;
            }
        }
    }
    // parse side to move
    fen++;
    board.side = (*fen == 'w') ? WHITE : BLACK;
    // parse castling rights
    fen += 2;
    while(*fen != ' ') {
        switch(*fen) {
            case 'K': board.castle |= wk; break;
            case 'Q': board.castle |= wq; break;
            case 'k': board.castle |= bk; break;
            case 'q': board.castle |= bq; break;
            default: break;
        }
        fen++;
    }
    // parse en passant square
    fen++;
    if (*fen != '-') {
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0');
        board.enpass = (rank<<3) + file;
        fen += 2;
    }
    // WHITE occupancies
    for (int piece = P; piece <= K; piece++)
        board.occ[WHITE] |= board.bb[piece];
    // BLACK occupancies
    for (int piece = p; piece <= k; piece++)
        board.occ[BLACK] |= board.bb[piece];
    board.occ[BOTH] = board.occ[WHITE] | board.occ[BLACK];
    // set position hash key
    board.hashKey = Zobrist::generateHashKey();
}

/// @brief parse a move written in UCI format
/// @param move_string 
/// @return 
int parseMove(const char* moveString) {
    MoveList moveList;
    MoveGen::generateMoves(moveList);
    int from = (moveString[0] - 'a') + ((8 - (moveString[1] - '0')) << 3),
        to   = (moveString[2] - 'a') + ((8 - (moveString[3] - '0')) << 3);
    // check if move is legal
    for (int i = 0; i < moveList.count; i++) {
        int move = moveList.moves[i];
        if(from == getFromSquare(move) && to == getToSquare(move)) {
            int promotedPiece = getPromotedPiece(move);
            // move is a promotion
            if (promotedPiece != blank) {
                char promotedChar = moveString[4];
                if ((promotedChar == 'q' && promotedPiece != (board.side == WHITE ? Q : q)) ||
                    (promotedChar == 'r' && promotedPiece != (board.side == WHITE ? R : r)) ||
                    (promotedChar == 'b' && promotedPiece != (board.side == WHITE ? B : b)) ||
                    (promotedChar == 'n' && promotedPiece != (board.side == WHITE ? N : n)))
                    continue;
            }
            return move;
        }
    }
    // illegal move
    return 0;
}

/// @brief parse UCI "position" command
/// @param command 
void parsePosition(const char *command) {
    command += 9; // skip "position "
    const char *currentChar = command;
    // "start" command
    if (strncmp(command, "startpos", 8) == 0)
        UCI::parseFEN(startFEN);
    else {
        currentChar = strstr(command, "fen");
        if (currentChar != NULL) {
            currentChar += 4; // skip "fen "
            UCI::parseFEN(currentChar);
        } else
            UCI::parseFEN(startFEN);
    }
    // parse moves after position
    currentChar = strstr(command, "moves");
    if (currentChar != NULL) {
        currentChar += 6; // skip "moves "
        while (*currentChar) {
            int move = parseMove(currentChar);
            if (move == 0)
                break;
            repetitionIndex++;
            repetitionTable[repetitionIndex] = board.hashKey;
            makeMove(move, 0);
            while (*currentChar && *currentChar != ' ')
                currentChar++;
            currentChar++; // ignore ' ' between moves
        }
    }
}

/// @brief parse UCI "go" command
/// @param command 
void parseGo(char* command) {
    int depth = -1;

    // reset variables
    Search::quit        = 0;
    Search::movestogo   = 30;
    Search::movetime    = -1;
    Search::time        = -1;
    Search::inc         = 0;
    Search::starttime   = 0;
    Search::stoptime    = 0;
    Search::timeset     = 0;
    Search::stopped     = 0;

    char *argument = NULL;
    
    // parse "infinite" command
    if (argument = strstr(command, "infinite")) {}
    // parse "binc" command (black time incement)
    if ((argument = strstr(command,"binc")) && board.side == BLACK)
        Search::inc = atoi(argument + 5);
    // parse "winc" command (white time incement)
    if ((argument = strstr(command,"winc")) && board.side == WHITE)
        Search::inc = atoi(argument + 5);
    // parse UCI "wtime" command (white time limit)
    if ((argument = strstr(command,"wtime")) && board.side == WHITE)
        Search::time = atoi(argument + 6);
    // parse UCI "btime" command (black time increment)
    if ((argument = strstr(command,"btime")) && board.side == BLACK)
        Search::time = atoi(argument + 6);
    // parse UCI "movestogo" command
    if ((argument = strstr(command,"movestogo")))
        Search::movestogo = atoi(argument + 10);
    // parse UCI "movetime" command
    if ((argument = strstr(command,"movetime")))
        Search::movetime = atoi(argument + 9);
    // parse UCI "depth" command
    if ((argument = strstr(command,"depth")))
        // parse search depth
        depth = atoi(argument + 6);
    // if move time is not available
    if(Search::movetime != -1) {
        // set time equal to move time
        Search::time = Search::movetime;
        // set moves to go to 1
        Search::movestogo = 1;
    }
    // init start time
    Search::starttime = getTimeMs();
    // init search depth
    depth = depth;
    // if time control is available
    if(Search::time != -1) {
        // flag we're playing with time control
        Search::timeset = 1;
        // set up timing
        Search::time /= Search::movestogo;
        Search::time -= 50;
        Search::stoptime = Search::starttime + Search::time + Search::inc;
    }
    // if depth is not available
    if(depth == -1)
        // set depth to 64 plies (basically inf)
        depth = 64;
    // print debug info
    printf("time:%d start:%d stop:%d depth:%d timeset:%d\n",
    Search::time, Search::starttime, Search::stoptime, depth, Search::timeset);
    // search position
    Search::searchPosition(depth);
}

void UCI::uciLoop() {
    // stdin, stdout buffers
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);
    // define user / GUI input buffer
    char input[2000];
    while (1) // main loop
    {
        memset(input, 0, sizeof(input)); // reset input buffer
        fflush(stdout);                  // clear output buffer
        if(!fgets(input, 2000, stdin) || input[0] == '\n')
            continue;
        // parse UCI "isready"
        if (strncmp(input, "isready", 7) == 0)
            printf("readyok\n");
        // parse UCI position command
        else if (strncmp(input, "position", 8) == 0)
            parsePosition(input);
        // parse UCI newgame command
        else if (strncmp(input, "ucinewgame", 10) == 0)
            parsePosition("position startpos");
        // parse UCI go command
        else if (strncmp(input, "go", 2) == 0)
            parseGo(input);
        // parse UCI quit command
        else if (strncmp(input, "quit", 4) == 0)
            break;
        // parse UCI uci command
        else if (strncmp(input, "uci", 3) == 0) {
            // print engine info
            printf("id name %s\n", NAME);
            printf("id author %s\n", AUTHOR);
            printf("uciok\n");
        }
    }
}
