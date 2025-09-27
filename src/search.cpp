#include "search.h"

// modern engines prefer fail soft cutoff, but this implementation is fail hard cutoff

namespace Search {

    int quit        = 0;
    int movestogo   = 30;
    int movetime    = -1;
    int time        = -1;
    int inc         = 0;
    int starttime   = 0;
    int stoptime    = 0;
    int timeset     = 0;
    int stopped     = 0;

    unsigned long long nodes;

    const int MVVLVA[12][12] = {
        105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
        104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
        103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
        102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
        101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
        100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

        105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
        104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
        103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
        102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
        101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
        100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
    };

    const int MVVLVAOffset = 10000;

    int killerMoves[2][64];
    int historyMoves[12][64];

    int pvTable[maxPly][maxPly];
    int pvLength[maxPly];

    int followPV, scorePV;

    /* MOVE ORDERING FUNCTIONS */

    /// @brief enable principle variation move scoring
    /// @param moveList 
    inline void enablePVScoring(MoveList &moveList) {
        followPV = 0;
        for (int i = 0; i < moveList.count; i++) {
            // check if PV node
            if (pvTable[0][ply] == moveList.moves[i]) {
                scorePV = 1;
                followPV = 1;
            }
        }
    }

    /// @brief score a move using the MVV-LVA & principle variation
    /// @param move 
    /// @return move score
    inline int scoreMove(int move) {
        // PV enabled
        if (scorePV) {
            if (pvTable[0][ply] == move) {
                scorePV = 0;
                return pvScore;
            }
        }
        // captures
        if (isCapture(move)) {
            int to = getToSquare(move);
            for (int piece = (board.side == WHITE ? p : P); piece <= (board.side == WHITE ? k : K); piece++)
                if (getBit(board.bb[piece], to))
                    return MVVLVA[getPiece(move)][piece] + MVVLVAOffset;
            return MVVLVA[p][P] + MVVLVAOffset;
        } else {
            if (killerMoves[0][ply] == move)
                return killerMove1Score;
            if (killerMoves[1][ply] == move)
                return killerMove2Score;
            return historyMoves[getPiece(move)][getToSquare(move)];
        }
        return 0;
    }

    /// @brief sort moves according to their move scores
    /// @param moveList 
    inline void sortMoves(MoveList &moveList) {
        int moveScores[256];
        for (int i = 0; i < moveList.count; i++)
            moveScores[i] = scoreMove(moveList.moves[i]);
        // n^2 sort
        for (int cur = 0; cur < moveList.count; cur++)
            for(int nxt = cur+1; nxt < moveList.count; nxt++)
                if (moveScores[cur] < moveScores[nxt]) {
                    // swap move
                    int tempMove = moveList.moves[cur];
                    moveList.moves[cur] = moveList.moves[nxt];
                    moveList.moves[nxt] = tempMove;
                    // swap score
                    int tempScore = moveScores[nxt];
                    moveScores[nxt] = moveScores[cur];
                    moveScores[cur] = tempScore;
                }
    }

    /// @brief detect if the current position is a repetition
    /// @return 
    inline bool isRepetition() {
        for (int i = 0; i < repetitionIndex; i++)
            if (repetitionTable[i] == board.hashKey)
                return 1;
        return 0;
    }

    /* MAIN SEARCH FUNCTIONS */

    /// @brief search until a "quiet" position, preventing the horizon effect
    /// @param alpha 
    /// @param beta 
    /// @return 
    inline int quiescence(int alpha, int beta) {

        // every 2047 nodes, check for stop command;
        if ((nodes & 2047) == 0) {
            UCI::communicate();
        }

        nodes++;

        // static evaluation of the position
        int evaluation = Evaluation::evaluate();

        // check if fail high
        if (evaluation >= beta)
            return beta;
        // position is better than expected
        if (evaluation > alpha)
            alpha = evaluation;
        MoveList moveList;
        MoveGen::generateMoves(moveList);
        sortMoves(moveList); // possible to use a captures only move gen (?)
        for (int i = 0; i < moveList.count; i++) {
            Board tempBoard = board;
            ply++;
            repetitionIndex++;
            repetitionTable[repetitionIndex] = board.hashKey;
            
            // only play capture moves
            // consider promotions as well (?)
            // consider checks as well (?)
            if (makeMove(moveList.moves[i], 1) == 0) {
                ply--;
                repetitionIndex--;
                continue;
            }
            int score = -quiescence(-beta, -alpha);
            board = tempBoard;
            ply--;
            repetitionIndex--;

            // check if time up
            if (stopped == 1)
                return 0;

            // position is better than expected
            if (score > alpha) {
                alpha = score;
                // fail high
                if (score >= beta)
                    return beta;
            }
        }
        return alpha;
    }

    /// @brief negative maximum implementation of minimax with alpha-beta pruning
    /// @param alpha 
    /// @param beta 
    /// @param depth 
    /// @return 
    inline int negamax(int alpha, int beta, int depth) {

        // every 2047 nodes, check for stop command;
        if ((nodes & 2047) == 0) {
            UCI::communicate();
        }

        int movesSearched = 0; // for PVS & LMR
        int score         = 0; // static evaluation score

        // assume default node fails
        int hashf = hfAlpha;

        // if repetition, return draw
        if (ply && isRepetition())
            return drawScore;
        
        // read hash table if,
        if (ply && // current node is not root node
            ((score = TT::readHashEntry(alpha, beta, depth)) != noHash) && // hash table entry exists
            (beta - alpha > 1) == 0) // current node is PV
            return score;

        // init PV length
        pvLength[ply] = ply;
        if (depth == 0) // base case
            return quiescence(alpha, beta);
        if (ply > maxPly - 1) // max ply reached
            return Evaluation::evaluate();
        nodes++;
        bool check = Attacks::isSquareAttacked(board.side == WHITE ? getLS1B(board.bb[K]) :
                                                                     getLS1B(board.bb[k]),
                                                                     board.side ^ 1);
        int legalMoves = 0;
        if (check)
            depth++;

        // null move pruning
        // search may prune away defensive moves
        // king safety is not considered (?)
        if (depth >= R && check == 0 && ply) {
            Board tempBoard = board;
            ply++;
            repetitionIndex++;
            repetitionTable[repetitionIndex] = board.hashKey;

            // make null move
            if (board.enpass != no_sq)
                board.hashKey ^= Zobrist::enpassantKeys[board.enpass];
            board.enpass = no_sq;
            board.side ^= 1;
            board.hashKey ^= Zobrist::sideKey;

            // reduce more aggressively if depth is high
            // could undervalue lines where a move opens up many lines & diagonals (?)
            int reduction = R; // = R + (depth > 6);
            score = -negamax(-beta, -beta + 1, depth-reduction);

            board = tempBoard;
            ply--;
            repetitionIndex--;

            // check if time up
            if (stopped == 1)
                return 0;

            // fail hard beta cutoff
            if (score >= beta)
                return beta;
        }

        MoveList moveList;
        MoveGen::generateMoves(moveList);
        if (followPV) // check if PV enabled
            enablePVScoring(moveList);
        sortMoves(moveList);

        for (int i = 0; i < moveList.count; i++) {
            Board tempBoard = board;
            ply++;
            repetitionIndex++;
            repetitionTable[repetitionIndex] = board.hashKey;

            if (makeMove(moveList.moves[i], 0) == 0) {
                ply--;
                repetitionIndex--;
                continue;
            }

            legalMoves++;

            // do full search
            if (movesSearched == 0)
                score = -negamax(-beta, -alpha, depth-1);
            else {
                // do PV search
                // late move reduction if,
                if (
                    movesSearched >= fullDepthMoves &&           // long enough principle variation exists 
                    depth >= R &&                                // far enough into search tree
                    check == 0 &&                                // is not check
                    isCapture(moveList.moves[i]) == 0 &&         // move is not captures
                    getPromotedPiece(moveList.moves[i]) == blank // no promotions
                )
                    score = -negamax(-alpha-1, -alpha, depth-2);
                else
                    score = alpha + 1; // ensure full search again
                
                if (score > alpha) {
                    // score shows promise
                    // do full search with closed aspiration window
                    score = -negamax(-alpha - 1, -alpha, depth - 1);

                    // fail high, do full search
                    if ((score > alpha) && (score < beta))
                        score = -negamax(-beta, -alpha, depth - 1);
                }
            }

            board = tempBoard;
            ply--;
            repetitionIndex--;

            // check if time up
            if (stopped == 1)
                return 0;

            movesSearched++;

            // better move found
            if (score > alpha) {
                hashf = hfExact;

                if (isCapture(moveList.moves[i]) == 0)
                    historyMoves[getPiece(moveList.moves[i])][getToSquare(moveList.moves[i])] += depth;

                // PV node score
                alpha = score;
                // PV node
                pvTable[ply][ply] = moveList.moves[i];

                // copy move from deeper ply into current ply's line
                for (int nextPly = ply + 1; nextPly < pvLength[ply + 1]; nextPly++)
                    pvTable[ply][nextPly] = pvTable[ply + 1][nextPly];
                
                // adjust PV length
                pvLength[ply] = pvLength[ply + 1];

                // fail hard beta cutoff
                if (score >= beta) {
                    TT::writeHashEntry(beta, depth, hfBeta);
                    // if quiet move
                    if (isCapture(moveList.moves[i]) == 0) {
                        // store killer move
                        killerMoves[1][ply] = killerMoves[0][ply];
                        killerMoves[0][ply] = moveList.moves[i];
                    }
                    return beta;
                }

            }
        }

        // checkmate / stalemate
        if (legalMoves == 0)
            return check ? -mateValue + ply : drawScore; // + ply finds shortest route to mate
        // node fails low
        TT::writeHashEntry(alpha, depth, hashf);
        return alpha;
    }

    int searchPosition(int depth) {
        nodes = followPV = scorePV = 0;

        // reset stop search flag
        stopped = 0;

        // clear helper data structures
        memset(killerMoves, 0, sizeof(killerMoves));
        memset(historyMoves, 0, sizeof(historyMoves));
        memset(pvTable, 0, sizeof(pvTable));
        memset(pvLength, 0, sizeof(pvLength));

        // alpha beta window
        int alpha = -inf, beta = inf, score = 0;

        // iterative deepening loop
        // principle variation helps prune branches in deeper plies
        for (int currentDepth = 1; currentDepth <= depth; currentDepth++) {

            // check if time up
            if (stopped == 1)
                break;

            followPV = 1;

            score = negamax(alpha, beta, currentDepth);
            // if outside window, search again with full width window
            if ((score <= alpha) || (score >= beta)) {
                alpha = -inf;
                beta = inf;
                // score = negamax(alpha, beta, currentDepth); (?)
                continue;
            }
            alpha = score - aspirationWindow;
            beta = score + aspirationWindow;

            // print evaluation scores, PV lines at each depth
            if (score > -mateValue && score < -mateScore)
                printf("info score mate %d depth %d nodes %llu pv ", -(score + mateValue) / 2 - 1, currentDepth, nodes);
            else if (score > mateScore && score < mateValue)
                printf("info score mate %d depth %d nodes %llu pv ", (mateValue - score) / 2 + 1, currentDepth, nodes);
            else
                printf("info score cp %d depth %d nodes %llu pv ", score, currentDepth, nodes);

            // loop over moves within a PV line
            for (int count = 0; count < pvLength[0]; count++) {
                printMove(pvTable[0][count]);
                printf(" ");
            }
            printf("\n");
        }

        printf("bestmove ");
        printMove(pvTable[0][0]);
        printf("\n");

        return pvTable[0][0];
    }

}