#include <stdint.h>
#include "../common/constants.hpp"

struct BoardState {
    unsigned __int128 players[2]; //Lowest 81 bits for whole board
    uint16_t globalBoards[2];
    uint16_t drawBoard;
    uint16_t currBoard; //One-Hot, All open boards hot
    uint16_t currPlayer;
};