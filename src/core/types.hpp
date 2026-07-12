#pragma once

#include <stdint.h>
#include "../common/constants.hpp"

struct BoardState {
    __uint128_t players[2] = {0, 0}; //Lowest 81 bits for whole board
    uint16_t globalBoards[2] = {0, 0};
    uint16_t drawBoard = 0;
    uint16_t currBoard = 0x1FF; //One-Hot, All open boards hot
    uint16_t currPlayer = 0;
};