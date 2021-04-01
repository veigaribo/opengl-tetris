#pragma once

#define FIELD_WIDTH 12
#define FIELD_HEIGHT 18

enum TileType {
  TILE_FREE = 0,
  TILE_OCCUPIED_BY_ACTIVE = 1,
  TILE_OCCUPIED_BY_STATIC = 2,
  TILE_WALL = 3
};

struct Tile {
  enum TileType type;
  float color[3];
};

unsigned int to1D(int x, int y);
