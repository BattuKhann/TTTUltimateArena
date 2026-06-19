#include "engine.hpp"

inline unsigned __int128 getOccupied(BoardState &state){
    return state.players[0] | state.players[1];
}

bool checkWin(uint16_t board){
    return  ((board & R1 == R1)) ||
            ((board & R2 == R2)) ||
            ((board & R3 == R3)) ||
            ((board & C1 == C1)) ||
            ((board & C2 == C2)) ||
            ((board & C3 == C3)) ||
            ((board & D1 == D1)) ||
            ((board & D2 == D2));
}

void move(BoardState &state, int square){
    unsigned __int128 targetMask = (unsigned __int128)1 << square;
    unsigned __int128 &currMask = state.players[state.currPlayer];
    currMask |= targetMask;

    //If current board is won or target is occupied
    if (checkWin(currMask << ((square / 9) * 9)) || (((targetMask = 1) << (112 + square % 9)) & getOccupied())) state.currBoard = 0xFF;
    else state.currBoard = square % 9;

    state.currPlayer = 1 - state.currPlayer;
}