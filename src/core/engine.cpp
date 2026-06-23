#include "engine.hpp"

inline __uint128_t getOccupied(const BoardState &state){
    return state.players[0] | state.players[1];
}

inline uint16_t getGlobalBoard(const BoardState &state, int player){
    return state.globalBoards[player];
}

inline uint16_t getGlobalBoard(const BoardState &state){
    return getGlobalBoard(state, 0) | getGlobalBoard(state, 1);
}

inline void setGlobalBoard(BoardState &state, int player, int board){
    state.globalBoards[player] |= (1 << board);
}

inline bool isFull(const BoardState &state, int board){
    return state.drawBoard & ((uint16_t)1 << board);
}

inline void setFull(BoardState &state, int board){
    state.drawBoard |= ((uint16_t)1 << board);
}

inline bool checkWin(uint16_t board){
    return  ((board & R1) == R1) ||
            ((board & R2) == R2) ||
            ((board & R3) == R3) ||
            ((board & C1) == C1) ||
            ((board & C2) == C2) ||
            ((board & C3) == C3) ||
            ((board & D1) == D1) ||
            ((board & D2) == D2);
}

inline bool checkFull(uint16_t board){
    return (board & 0x01FF) == 0x01FF;
}

inline __uint128_t getLegalMoves(const BoardState &state){
    __uint128_t legal = 0;
    uint16_t open = state.currBoard;

    legal |= (__uint128_t)(-((open >> 0) & 1) & 0x1FF) << 0;
    legal |= (__uint128_t)(-((open >> 1) & 1) & 0x1FF) << 9;
    legal |= (__uint128_t)(-((open >> 2) & 1) & 0x1FF) << 18;
    legal |= (__uint128_t)(-((open >> 3) & 1) & 0x1FF) << 27;
    legal |= (__uint128_t)(-((open >> 4) & 1) & 0x1FF) << 36;
    legal |= (__uint128_t)(-((open >> 5) & 1) & 0x1FF) << 45;
    legal |= (__uint128_t)(-((open >> 6) & 1) & 0x1FF) << 54;
    legal |= (__uint128_t)(-((open >> 7) & 1) & 0x1FF) << 63;
    legal |= (__uint128_t)(-((open >> 8) & 1) & 0x1FF) << 72;

    return legal & ~getOccupied(state);
}

bool move(BoardState &state, int square){
    int currPlayer = state.currPlayer;
    __uint128_t &currMask = state.players[currPlayer];
    __uint128_t targetMask = (__uint128_t)1 << square;
    currMask |= targetMask;

    int currBoard = square / 9;
    int nextBoard = square % 9;

    bool win = checkWin(currMask >> (currBoard * 9));
    bool full = !win && checkFull(getOccupied(state) >> (currBoard * 9));

    state.globalBoards[currPlayer] |= ((uint16_t)(win) << currBoard);
    state.drawBoard |= ((uint16_t)(full) << currBoard);

    uint16_t deadBoards = (getGlobalBoard(state) | state.drawBoard);
    bool isDead = (deadBoards >> nextBoard) & 1;

    state.currBoard = ((-isDead) & (~deadBoards & 0x01FF)) | ((!isDead) << nextBoard);

    state.currPlayer = 1 - currPlayer;
    return checkWin(getGlobalBoard(state, currPlayer));
}