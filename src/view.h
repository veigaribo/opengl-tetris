#ifndef VIEW_H
#define VIEW_H

#include "game.h"

struct Vertex {
  // position
  float x;
  float y;

  // index into the COLORS array
  unsigned char color;
};

void fancyRenderInit();
void fancyRender(struct GameState *game);
void terminateRender();

#endif /* VIEW_H */
