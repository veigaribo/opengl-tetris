#include "game.h"
#include "config.h"
#include "field.h"
#include "piece.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct PieceTemplate pieceTemplates[7];

static inline void fillWalls(struct GameState *game) {
  struct Tile *field = game->field;

  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      unsigned int fieldIndex = to1D(x, y);

      bool isLeftWall = x == 0;
      bool isRightWall = x == FIELD_WIDTH - 1;
      bool isBottomWall = y == 0;

      if (isLeftWall || isRightWall || isBottomWall) {
        struct Tile tile;
        tile.type = TILE_WALL;
        tile.color[0] = WALL_COLOR;
        tile.color[1] = WALL_COLOR;
        tile.color[2] = WALL_COLOR;

        field[fieldIndex] = tile;
      };
    }
  }
}

static inline void fillActivePiece(struct GameState *game) {
  struct ActivePiece piece = game->currentPiece;
  struct Tile *field = game->field;

  for (int py = 0; py < 4; py++) {
    for (int px = 0; px < 4; px++) {
      unsigned int pieceIndex = rotatedIndex(px, py, piece.rotation);

      bool containsPiece = piece.pieceTemplate.map[pieceIndex];

      if (containsPiece) {
        unsigned int fieldIndex = to1D(piece.x + px, piece.y - py);

        struct Tile tile;
        tile.type = TILE_OCCUPIED_BY_ACTIVE;
        memcpy(tile.color, piece.pieceTemplate.color, sizeof(float) * 3);

        field[fieldIndex] = tile;
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

      if (tile.type == TILE_OCCUPIED_BY_ACTIVE) {
        field[fieldIndex].type = TILE_FREE;
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
          bool containsPiece = piece->pieceTemplate.map[pieceIndex];

          enum TileType tileType = field[fieldIndex].type;

          if (containsPiece && tileType != TILE_FREE &&
              tileType != TILE_OCCUPIED_BY_ACTIVE) {
            return false;
          }
        }
    }
  }

  return true;
}

static inline struct ActivePiece newPiece() {
  struct ActivePiece piece;
  piece.x = FIELD_WIDTH / 2;
  piece.y = FIELD_HEIGHT - 1;
  piece.rotation = 0;

  unsigned int random = rand() % 7;
  piece.pieceTemplate = pieceTemplates[random];

  return piece;
}

struct GameState *start(GLFWwindow *window) {
  struct PieceTemplate pieceTemplatesTmp[] = {
      PIECE_O, PIECE_L, PIECE_J, PIECE_I, PIECE_S, PIECE_Z, PIECE_T};
  memcpy(pieceTemplates, pieceTemplatesTmp, sizeof(struct PieceTemplate) * 7);

  unsigned int FIELD_SIZE = sizeof(struct Tile[FIELD_WIDTH * FIELD_HEIGHT]);

  struct Tile field[FIELD_WIDTH * FIELD_HEIGHT];
  for (int i = 0; i < FIELD_WIDTH * FIELD_HEIGHT; i++) {
    struct Tile tile;
    tile.type = TILE_FREE;

    field[i] = tile;
  }

  struct ActivePiece initialPiece = newPiece();
  int linesToClean[4] = {-1, -1, -1, -1};

  struct GameState *gameState = calloc(1, sizeof(struct GameState));
  gameState->window = window;
  gameState->gameOver = false;
  gameState->currentPiece = initialPiece; // copy by value
  gameState->pieceCount = 0;
  gameState->speed = INITIAL_SPEED;
  gameState->speedCounter = 0;
  gameState->forceDown = false;
  gameState->score = 0;

  memcpy(&gameState->linesToClean, linesToClean, sizeof(int[4]));
  memcpy(&gameState->field, field, FIELD_SIZE);

  fillWalls(gameState);
  fillActivePiece(gameState);

  return gameState;
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

          if (tile.type == TILE_OCCUPIED_BY_ACTIVE) {
            game->field[fieldIndex].type = TILE_OCCUPIED_BY_STATIC;
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

          isLine &= tile.type != TILE_FREE;
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

      struct ActivePiece piece = newPiece();

      game->currentPiece = piece;

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

void end(struct GameState *game) { free(game); }
