#ifndef GAME_H
#define GAME_H

#include "common.h"
#include "field.h"
#include "piece.h"
#include <GLFW/glfw3.h>

struct GameState {
  GLFWwindow *window;

  struct Tile field[FIELD_WIDTH * FIELD_HEIGHT];
  struct ActivePiece currentPiece;

  float forceDownInterval;
  float speedCounter;

  uint32_t pieceCount;
  uint32_t score;

  bool gameOver;
};

void start(GLFWwindow *window, struct GameState *game);
void update(struct GameState *game, double dt);
void end(struct GameState *game);

#endif /* GAME_H */
