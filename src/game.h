#ifndef GAME_H
#define GAME_H

#include "common.h"
#include "field.h"
#include "piece.h"
#include <GLFW/glfw3.h>

struct GameState {
  GLFWwindow *window;

  struct Tile field[FIELD_WIDTH * FIELD_HEIGHT];

  bool gameOver;

  struct ActivePiece currentPiece;

  unsigned int pieceCount;

  float forceDownInterval;
  float speedCounter;

  unsigned int score;
};

void start(GLFWwindow *window, struct GameState *game);
void update(struct GameState *game, double dt);
void end(struct GameState *game);

#endif /* GAME_H */
