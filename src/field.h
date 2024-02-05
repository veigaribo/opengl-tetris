#ifndef FIELD_H
#define FIELD_H

#include "common.h"
#define FIELD_WIDTH 12
#define FIELD_HEIGHT 18

enum {
  TILE_FREE = 0 << 0,
  TILE_OCCUPIED_BY_ACTIVE = 1 << 0,
  TILE_OCCUPIED_BY_STATIC = 2 << 0,
  TILE_WALL = 3 << 0
};

// See the Tile.value comment for more info
#define IS_OCCUPIED_BY_ACTIVE(Var) ((Var & 0b11) == 1)
#define IS_OCCUPIED_BY_STATIC(Var) ((Var & 0b11) == 2)

#define GET_PIECE_TYPE(Var) (Var & 0b11100)

// Converts from OCCUPIED_BY_ACTIVE to OCCUPIED_BY_STATIC
// keeping the piece type
#define TO_STATIC(Var) (Var ^ 3)

struct Tile {
  // This should be:
  // Either TILE_FREE or TILE_WALL
  // or one of the others bitwise ORed with a piece type
  // (see piece.h)
  uint8_t value;
};

uint8_t to1D(uint8_t x, uint8_t y);

#endif /* FIELD_H */
