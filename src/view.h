#ifndef VIEW_H
#define VIEW_H

#include "game.h"

struct Vertex {
  // position
  float x;
  float y;
};

void renderInit();
void render(struct GameState *game);
void terminateRender();

#endif /* VIEW_H */
