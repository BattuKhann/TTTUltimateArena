#include <stdint.h>
#include "../common/constants.hpp"

#define GLOBAL_BOARD_MASK (unsigned __int128)0b111111111 << 119

struct Result {
    uint8_t targetBoard;
    bool taken;
};

struct BoardState {
    unsigned __int128 unoccupied; //Boards use 9 MSB for Global Board
    unsigned __int128 p1;
    unsigned __int128 p2;
    uint8_t currBoard;
    uint8_t currPlayer;
};