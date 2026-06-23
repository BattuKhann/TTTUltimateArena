#include "types.hpp"

inline __uint128_t getOccupied(const BoardState &state);

inline uint16_t getGlobalBoard(const BoardState &state, int player);

inline uint16_t getGlobalBoard(const BoardState &state);

inline void setGlobalBoard(BoardState &state, int player, int board);

inline bool isFull(const BoardState &state, int board);

inline void setFull(BoardState &state, int board);

inline bool checkWin(uint16_t board);

inline bool checkFull(uint16_t board);

inline __uint128_t getLegalMoves(const BoardState &state);

bool move(BoardState &state, int square); //Sets currboard to 0xFF for open play, returns true if game is won