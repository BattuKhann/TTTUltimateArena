#include "engine.hpp"

int8_t move(BoardState &state, int square){
    static constexpr uint8_t DIV9[81] = {
        0,0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1,1, 2,2,2,2,2,2,2,2,2,
        3,3,3,3,3,3,3,3,3, 4,4,4,4,4,4,4,4,4, 5,5,5,5,5,5,5,5,5,
        6,6,6,6,6,6,6,6,6, 7,7,7,7,7,7,7,7,7, 8,8,8,8,8,8,8,8,8
    };
    static constexpr uint8_t MOD9[81] = {
        0,1,2,3,4,5,6,7,8, 0,1,2,3,4,5,6,7,8, 0,1,2,3,4,5,6,7,8,
        0,1,2,3,4,5,6,7,8, 0,1,2,3,4,5,6,7,8, 0,1,2,3,4,5,6,7,8,
        0,1,2,3,4,5,6,7,8, 0,1,2,3,4,5,6,7,8, 0,1,2,3,4,5,6,7,8
    };

    int currPlayer = state.currPlayer;
    __uint128_t &currMask = state.players[currPlayer];
    __uint128_t targetMask = (__uint128_t)1 << square;
    currMask |= targetMask;

    int currBoard = DIV9[square];
    int nextBoard = MOD9[square];

    int boardShift = square - nextBoard;

    bool win = checkWin(currMask >> boardShift);
    bool full = !win & checkFull(getOccupied(state) >> boardShift);

    state.globalBoards[currPlayer] |= ((uint16_t)(win) << currBoard);
    state.drawBoard |= ((uint16_t)(full) << currBoard);

    uint16_t deadBoards = getGlobalBoard(state);
    bool isDead = (deadBoards >> nextBoard) & 1;

    state.currBoard = ((-isDead) & (~deadBoards & 0x01FF)) | ((!isDead) << nextBoard);

    bool gameOver = checkWin(getGlobalBoard(state, currPlayer));
    bool globalDraw = ((deadBoards & 0x01FF) == 0x01FF);

    state.currPlayer = 1 - currPlayer;
    return (globalDraw << 1) | gameOver;
}