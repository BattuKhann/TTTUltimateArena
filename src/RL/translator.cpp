#include "translator.hpp"

void writeStateVector(const BoardState& state, int8_t* outBuffer){
    int currPlayer = state.currPlayer;
    __uint128_t friendly = state.players[currPlayer];
    __uint128_t enemy = state.players[1 - currPlayer];
    __uint128_t legalMoves = getLegalMoves(state);
    __uint128_t target = 1;

    for(int i = 0; i < 81; i++){
        int idx = i*3;
        outBuffer[idx] = (bool)(friendly & target);
        outBuffer[idx + 1] = (bool)(enemy & target);
        outBuffer[idx + 2] = (bool)(legalMoves & target);
        target <<= 1;
    }

    outBuffer += 81 * 3;
    uint16_t currBoard = state.currBoard;
    uint16_t friendlyGlobal = state.globalBoards[currPlayer];
    uint16_t enemyGlobal = state.globalBoards[1 - currPlayer];
    
    for(int i = 0; i < 9; i++){
        outBuffer[i] = (currBoard >> i) & 1;
        outBuffer[i + 9] = (friendlyGlobal >> i) & 1;
        outBuffer[i + 18] = (enemyGlobal >> i) & 1;
    }
}