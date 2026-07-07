#include "engine.hpp"

int8_t move(BoardState &state, int square){
    if(state.winner) return 0;
    uint8_t reward;
    int currPlayer = state.currPlayer;
    __uint128_t &currMask = state.players[currPlayer];
    __uint128_t targetMask = (__uint128_t)1 << square;
    currMask |= targetMask;

    int currBoard = square / 9;
    int nextBoard = square % 9;

    bool win = checkWin(currMask >> (currBoard * 9));
    bool full = !win && checkFull(getOccupied(state) >> (currBoard * 9));
    reward = win * 0.1f;

    state.globalBoards[currPlayer] |= ((uint16_t)(win) << currBoard);
    state.drawBoard |= ((uint16_t)(full) << currBoard);

    uint16_t deadBoards = (getGlobalBoard(state) | state.drawBoard);
    bool isDead = (deadBoards >> nextBoard) & 1;

    state.currBoard = ((-isDead) & (~deadBoards & 0x01FF)) | ((!isDead) << nextBoard);

    state.currPlayer = 1 - currPlayer;
    return reward + checkWin(getGlobalBoard(state, currPlayer));
}