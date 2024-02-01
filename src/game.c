#include "game.h"
#include "config.h"
#include "field.h"
#include "piece.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct PieceTemplate *pieceTemplates[7];

static inline void fillWalls(struct GameState *game) {
  struct Tile *field = game->field;

  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      unsigned int fieldIndex = to1D(x, y);

      bool isLeftWall = x == 0;
      bool isRightWall = x == FIELD_WIDTH - 1;
      bool isBottomWall = y == 0;

      if (isLeftWall || isRightWall || isBottomWall) {
        field[fieldIndex].value = TILE_WALL;
      };
    }
  }
}

static inline void fillActivePiece(struct GameState *game) {
  struct ActivePiece *piece = &game->currentPiece;
  struct Tile *field = game->field;

  for (int py = 0; py < 4; py++) {
    for (int px = 0; px < 4; px++) {
      unsigned int pieceIndex = rotatedIndex(px, py, piece->rotation);

      bool containsPiece = piece->pieceTemplate->map[pieceIndex];

      if (containsPiece) {
        unsigned int fieldIndex = to1D(piece->x + px, piece->y - py);
        unsigned int pieceType = piece->pieceTemplate->type;

        field[fieldIndex].value = TILE_OCCUPIED_BY_ACTIVE | pieceType;
      };
    }
  }
}

static inline void clearCurrentPiece(struct GameState *game) {
  struct Tile *field = game->field;

  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      unsigned int fieldIndex = to1D(x, y);
      struct Tile tile = field[fieldIndex];

      if (IS_OCCUPIED_BY_ACTIVE(tile.value)) {
        field[fieldIndex].value = TILE_FREE;
      }
    }
  }
}

static inline bool doesPieceFit(struct GameState *game, unsigned int px,
                                unsigned int py, unsigned int r) {
  struct ActivePiece *piece = &game->currentPiece;
  struct Tile *field = game->field;

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      unsigned int pieceIndex = rotatedIndex(x, y, r);
      unsigned int fieldIndex = to1D(x + px, py - y);

      if (px + x >= 0 && px + x < FIELD_WIDTH)
        if (py - y >= 0 && py - y < FIELD_HEIGHT) {
          bool containsPiece = piece->pieceTemplate->map[pieceIndex];

          unsigned int tile = field[fieldIndex].value;

          if (containsPiece && tile != TILE_FREE &&
              !IS_OCCUPIED_BY_ACTIVE(tile)) {
            return false;
          }
        }
    }
  }

  return true;
}

static inline void newPiece(struct GameState *game) {
  game->currentPiece.x = FIELD_WIDTH / 2;
  game->currentPiece.y = FIELD_HEIGHT - 1;
  game->currentPiece.rotation = 0;

  unsigned int random = rand() % 7;
  game->currentPiece.pieceTemplate = pieceTemplates[random];
}

void start(GLFWwindow *window, struct GameState *game) {
  pieceTemplates[0] = &PIECE_O;
  pieceTemplates[1] = &PIECE_L;
  pieceTemplates[2] = &PIECE_J;
  pieceTemplates[3] = &PIECE_I;
  pieceTemplates[4] = &PIECE_S;
  pieceTemplates[5] = &PIECE_Z;
  pieceTemplates[6] = &PIECE_T;

  newPiece(game);
  game->window = window;
  game->gameOver = false;
  game->pieceCount = 0;
  game->speed = INITIAL_SPEED;
  game->speedCounter = 0;
  game->forceDown = false;
  game->score = 0;
  game->linesToClean[0] = -1;
  game->linesToClean[1] = -1;
  game->linesToClean[2] = -1;
  game->linesToClean[3] = -1;

  for (int i = 0; i < FIELD_WIDTH * FIELD_HEIGHT; i++) {
    game->field[i].value = TILE_FREE;
  }

  fillWalls(game);
  fillActivePiece(game);
}

void update(struct GameState *game, double dt) {
  if (game->gameOver) {
    return;
  }

  GLFWwindow *window = game->window;
  int leftInputState = glfwGetKey(window, GLFW_KEY_A);
  int rightInputState = glfwGetKey(window, GLFW_KEY_D);
  int downInputState = glfwGetKey(window, GLFW_KEY_S);
  int rotateInputState = glfwGetKey(window, GLFW_KEY_Z);

  bool userMoved =
      leftInputState || rightInputState || downInputState || rotateInputState;

  struct ActivePiece currentPiece = game->currentPiece;

  if (!game->holdingLeft && leftInputState &&
      doesPieceFit(game, currentPiece.x - 1, currentPiece.y,
                   currentPiece.rotation)) {
    game->currentPiece.x--;
  }

  if (!game->holdingRight && rightInputState &&
      doesPieceFit(game, currentPiece.x + 1, currentPiece.y,
                   currentPiece.rotation)) {
    game->currentPiece.x++;
  }

  if (!game->holdingDown && downInputState &&
      doesPieceFit(game, currentPiece.x, currentPiece.y - 1,
                   currentPiece.rotation)) {
    game->currentPiece.y--;
  }

  if (!game->holdingRotate && rotateInputState &&
      doesPieceFit(game, currentPiece.x, currentPiece.y,
                   currentPiece.rotation + 1)) {
    game->currentPiece.rotation++;
  }

  if (userMoved) {
    clearCurrentPiece(game);
    fillActivePiece(game);
  }

  game->holdingLeft = leftInputState;
  game->holdingRight = rightInputState;
  game->holdingDown = downInputState;
  game->holdingRotate = rotateInputState;

  game->speedCounter += dt;

  if (game->speedCounter > game->speed) {
    game->speedCounter -= game->speed;

    game->forceDown = true;
  }

  if (game->forceDown) {
    if (doesPieceFit(game, currentPiece.x, currentPiece.y - 1,
                     currentPiece.rotation)) {
      game->currentPiece.y--;

      clearCurrentPiece(game);
    } else {
      // make active piece static
      for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
          unsigned int fieldIndex = to1D(x, y);
          struct Tile tile = game->field[fieldIndex];

          if (IS_OCCUPIED_BY_ACTIVE(tile.value)) {
            game->field[fieldIndex].value = TO_STATIC(tile.value);
          }
        }
      }

      unsigned int linesCompleted = 0;

      // check for line completion
      for (int y = 1; y <= game->currentPiece.y; y++) {
        bool isLine = true;

        // no need to check the walls
        for (int x = 1; x < FIELD_WIDTH - 1; x++) {
          unsigned int fieldIndex = to1D(x, y);
          struct Tile tile = game->field[fieldIndex];

          isLine &= tile.value != TILE_FREE;
        }

        if (isLine) {
          game->linesToClean[linesCompleted] = y;
          linesCompleted++;
        }
      }

      game->pieceCount++;
      if (game->pieceCount % PIECES_FOR_SPEED_INCREASE == 0 &&
          game->speed > MAX_SPEED) {
        game->speed -= SPEED_VARIATION;
      }

      if (linesCompleted > 0) {
        int *linesToClean = game->linesToClean;

        for (int i = linesCompleted - 1; i >= 0; i--) {
          int line = linesToClean[i];

          for (int x = 1; x < FIELD_WIDTH - 1; x++) {
            for (int y = linesToClean[i]; y < FIELD_HEIGHT - 1; y++) {
              unsigned int fieldIndex = to1D(x, y);
              unsigned int fieldIndexAbove = to1D(x, y + 1);

              game->field[fieldIndex] = game->field[fieldIndexAbove];
            }
          }

          linesToClean[i] = -1;
        }
      }

      game->score += SCORE_PER_PIECE;
      if (linesCompleted > 0) {
        game->score += SCORE_PER_LINES(linesCompleted);
      };

      printf("Score: %d\n", game->score);

      newPiece(game);

      // if the piece has just spawned and does not fit, game over
      if (!doesPieceFit(game, game->currentPiece.x, game->currentPiece.y,
                        game->currentPiece.rotation)) {
        game->gameOver = true;

        printf("Game Over!! Score: %d\n", game->score);
      }
    }

    fillActivePiece(game);
    game->forceDown = false;
  }
}

void end(struct GameState *game) {}
