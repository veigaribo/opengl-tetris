#include "game.h"
#include "config.h"
#include "field.h"
#include "input.h"
#include "piece.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct PieceTemplate *pieceTemplates[7];

static inline void fillWalls(struct GameState *game) {
  struct Tile *field = game->field;

  for (uint8_t y = 0; y < FIELD_HEIGHT; y++) {
    for (uint8_t x = 0; x < FIELD_WIDTH; x++) {
      uint8_t fieldIndex = to1D(x, y);

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

  for (uint8_t py = 0; py < 4; py++) {
    for (uint8_t px = 0; px < 4; px++) {
      uint8_t pieceIndex = rotatedIndex(px, py, piece->rotation);
      bool containsPiece = piece->pieceTemplate->map[pieceIndex];

      if (containsPiece) {
        uint8_t fieldIndex = to1D(piece->x + px, piece->y + py);
        uint8_t pieceType = piece->pieceTemplate->type;

        field[fieldIndex].value = TILE_OCCUPIED_BY_ACTIVE | pieceType;
      };
    }
  }
}

static inline void clearCurrentPiece(struct GameState *game) {
  struct Tile *field = game->field;

  for (uint8_t y = 0; y < FIELD_HEIGHT; y++) {
    for (uint8_t x = 0; x < FIELD_WIDTH; x++) {
      uint8_t fieldIndex = to1D(x, y);
      struct Tile tile = field[fieldIndex];

      if (IS_OCCUPIED_BY_ACTIVE(tile.value)) {
        field[fieldIndex].value = TILE_FREE;
      }
    }
  }
}

static inline bool doesPieceFit(struct GameState *game, uint8_t px, uint8_t py,
                                uint8_t r) {
  struct ActivePiece *piece = &game->currentPiece;
  struct Tile *field = game->field;

  for (uint8_t y = 0; y < 4; y++) {
    for (uint8_t x = 0; x < 4; x++) {
      uint8_t rotated = rotatedIndex(x, y, piece->rotation);
      uint8_t isFilled = piece->pieceTemplate->map[rotated];

      if (!isFilled)
        continue;

      uint8_t tileX = px + x;
      uint8_t tileY = py + y;

      if (tileX >= FIELD_WIDTH) {
        continue;
      }

      uint8_t fieldIndex = to1D(tileX, tileY);
      uint8_t tile = field[fieldIndex].value;

      if (tile != TILE_FREE && !IS_OCCUPIED_BY_ACTIVE(tile)) {
        return false;
      }
    }
  }

  return true;
}

static inline void newPiece(struct GameState *game) {
  game->currentPiece.x = FIELD_WIDTH / 2;
  game->currentPiece.y = FIELD_HEIGHT - 4 - 1;
  game->currentPiece.rotation = 0;

  uint8_t random = rand() % 7;
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
  game->forceDownInterval = INITIAL_DOWN_INTERVAL;
  game->speedCounter = 0;
  game->score = 0;

  for (uint8_t i = 0; i < FIELD_WIDTH * FIELD_HEIGHT; i++) {
    game->field[i].value = TILE_FREE;
  }

  fillWalls(game);
  fillActivePiece(game);
}

uint8_t linesToClean[4];
bool forceDown;

void update(struct GameState *game, double dt) {
  if (game->gameOver) {
    return;
  }

  GLFWwindow *window = game->window;
  enum Input input = getInput(window);

  bool userMoved = input != INPUT_NONE;

  struct ActivePiece currentPiece = game->currentPiece;

  switch (input) {
  case INPUT_LEFT:
    if (doesPieceFit(game, currentPiece.x - 1, currentPiece.y,
                     currentPiece.rotation)) {
      game->currentPiece.x--;

      clearCurrentPiece(game);
      fillActivePiece(game);
    }
    break;
  case INPUT_RIGHT:
    if (doesPieceFit(game, currentPiece.x + 1, currentPiece.y,
                     currentPiece.rotation)) {
      game->currentPiece.x++;

      clearCurrentPiece(game);
      fillActivePiece(game);
    }
    break;
  case INPUT_DOWN:
    if (doesPieceFit(game, currentPiece.x, currentPiece.y - 1,
                     currentPiece.rotation)) {
      game->currentPiece.y--;

      clearCurrentPiece(game);
      fillActivePiece(game);
    }
    break;
  case INPUT_ROTATE:
    if (doesPieceFit(game, currentPiece.x, currentPiece.y,
                     currentPiece.rotation + 1)) {
      game->currentPiece.rotation = (game->currentPiece.rotation + 1) % 4;

      clearCurrentPiece(game);
      fillActivePiece(game);
    }
    break;
  default:
    break;
  }

  game->speedCounter += dt;

  if (game->speedCounter > game->forceDownInterval) {
    game->speedCounter -= game->forceDownInterval;
    forceDown = true;
  }

  if (forceDown) {
    if (doesPieceFit(game, currentPiece.x, currentPiece.y - 1,
                     currentPiece.rotation)) {
      // Do not move if moved already
      if (input != INPUT_DOWN) {
        game->currentPiece.y--;
        clearCurrentPiece(game);
      }
    } else {
      // make active piece static
      for (uint8_t y = 0; y < FIELD_HEIGHT; y++) {
        for (uint8_t x = 0; x < FIELD_WIDTH; x++) {
          uint8_t fieldIndex = to1D(x, y);
          struct Tile tile = game->field[fieldIndex];

          if (IS_OCCUPIED_BY_ACTIVE(tile.value)) {
            game->field[fieldIndex].value = TO_STATIC(tile.value);
          }
        }
      }

      size_t linesCompleted = 0;
      uint8_t scanUntil = game->currentPiece.y + 4;

      // check for line completion
      for (uint8_t y = 1; y <= scanUntil; y++) {
        bool isLine = true;

        // no need to check the walls
        for (uint8_t x = 1; x < FIELD_WIDTH - 1; x++) {
          uint8_t fieldIndex = to1D(x, y);
          struct Tile tile = game->field[fieldIndex];

          isLine = isLine && tile.value != TILE_FREE;
        }

        if (isLine) {
          linesToClean[linesCompleted] = y;
          linesCompleted++;
        }
      }

      game->pieceCount++;
      if (game->pieceCount % PIECES_FOR_SPEED_INCREASE == 0 &&
          game->forceDownInterval > MAX_DOWN_INTERVAL) {
        game->forceDownInterval -= DOWN_INTERVAL_VARIATION;
      }

      if (linesCompleted > 0) {
        // `i < max` = `i >= 0`
        for (uint8_t i = linesCompleted - 1; i < 255; i--) {
          uint8_t line = linesToClean[i];
          for (uint8_t x = 1; x < FIELD_WIDTH - 1; x++) {
            for (uint8_t y = linesToClean[i]; y < FIELD_HEIGHT - 1; y++) {
              uint8_t fieldIndex = to1D(x, y);
              uint8_t fieldIndexAbove = to1D(x, y + 1);

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
    forceDown = false;
  }
}

void end(struct GameState *game) {}
