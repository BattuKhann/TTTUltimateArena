#include <stdint.h>
#include "../common/constants.hpp"

#define GLOBAL_BOARD_MASK (unsigned __int128)0b0000000111111111 << 112

struct BoardState {
    unsigned __int128 players[2]; //Boards use 9 MSB for Global Board
    uint8_t currBoard; //0xFF if all boards available
    uint8_t currPlayer;
};