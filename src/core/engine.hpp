#include "types.hpp"

inline unsigned __int128 getOccupied();

bool checkWin(uint16_t board);

void move(BoardState &state, int square); //Sets currboard to 0xFF for open play