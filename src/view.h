#pragma once

#include "game.h"

struct Vertex {
  // position
  float x;
  float y;
};

void renderInit();
void render(struct GameState *game);
void terminateRender();
