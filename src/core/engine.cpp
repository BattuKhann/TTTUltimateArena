#include "engine.hpp"

Result move(BoardState &state, int square){
    unsigned __int128 targetMask = (unsigned __int128)1 << square;
    if (state.currPlayer == 1){
        state.p1 |= targetMask;
    } else if (state.currPlayer == 2){
        state.p2 |= targetMask;
    }

    state.unoccupied &= ~targetMask;
    return Result {(uint8_t)(square % 9), true};
}