#pragma once

#include "common.h"
#include "field.h"
#include "piece.h"
#include <GLFW/glfw3.h>

struct GameState {
  GLFWwindow *window;

  bool holdingLeft;
  bool holdingRight;
  bool holdingDown;
  bool holdingRotate;

  struct Tile field[FIELD_WIDTH * FIELD_HEIGHT];

  bool gameOver;

  struct ActivePiece currentPiece;

  unsigned int pieceCount;

  float speed;
  float speedCounter;

  bool forceDown;

  unsigned int score;

  int linesToClean[4];
};

struct GameState *start(GLFWwindow *window);
void update(struct GameState *game, double dt);
void end(struct GameState *game);
