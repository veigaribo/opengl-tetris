#include "piece.h"
#include "common.h"
#include "view.h"
#include <string.h>

// clang-format off
bool pieceOMap[] = {
  0, 0, 0, 0,
  0, 1, 1, 0,
  0, 1, 1, 0,
  0, 0, 0, 0
};
float pieceOColor[] = {0.9f, 0.9f, 0.1f};

bool pieceLMap[] = {
  0, 0, 0, 0,
  0, 1, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0
};
float pieceLColor[] = {0.9f, 0.5f, 0.1f};

bool pieceJMap[] = {
  0, 0, 0, 0,
  0, 1, 1, 0,
  0, 1, 0, 0,
  0, 1, 0, 0
};
float pieceJColor[] = {0.1f, 0.1f, 0.9f};

bool pieceIMap[] = {
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0,
  0, 0, 1, 0
};
float pieceIColor[] = {0.2f, 0.7f, 0.9f};

bool pieceSMap[] = {
  0, 1, 0, 0,
  0, 1, 1, 0,
  0, 0, 1, 0,
  0, 0, 0, 0
};
float pieceSColor[] = {0.2f, 0.9f, 0.2f};

bool pieceZMap[] = {
  0, 0, 1, 0,
  0, 1, 1, 0,
  0, 1, 0, 0,
  0, 0, 0, 0
};
float pieceZColor[] = {0.9f, 0.1f, 0.1f};

bool pieceTMap[] = {
  0, 0, 1, 0,
  0, 1, 1, 0,
  0, 0, 1, 0,
  0, 0, 0, 0
};
float pieceTColor[] = {0.8f, 0.1f, 0.8f};
// clang-format on

struct PieceTemplate PIECE_O;
struct PieceTemplate PIECE_L;
struct PieceTemplate PIECE_J;
struct PieceTemplate PIECE_I;
struct PieceTemplate PIECE_S;
struct PieceTemplate PIECE_Z;
struct PieceTemplate PIECE_T;

void initPieces() {
  memcpy(&PIECE_O.map, pieceOMap, sizeof(bool) * 4 * 4);
  memcpy(&PIECE_O.color, pieceOColor, sizeof(float) * 3);

  memcpy(&PIECE_L.map, pieceLMap, sizeof(bool) * 4 * 4);
  memcpy(&PIECE_L.color, pieceLColor, sizeof(float) * 3);

  memcpy(&PIECE_J.map, pieceJMap, sizeof(bool) * 4 * 4);
  memcpy(&PIECE_J.color, pieceJColor, sizeof(float) * 3);

  memcpy(&PIECE_I.map, pieceIMap, sizeof(bool) * 4 * 4);
  memcpy(&PIECE_I.color, pieceIColor, sizeof(float) * 3);

  memcpy(&PIECE_S.map, pieceSMap, sizeof(bool) * 4 * 4);
  memcpy(&PIECE_S.color, pieceSColor, sizeof(float) * 3);

  memcpy(&PIECE_Z.map, pieceZMap, sizeof(bool) * 4 * 4);
  memcpy(&PIECE_Z.color, pieceZColor, sizeof(float) * 3);

  memcpy(&PIECE_T.map, pieceTMap, sizeof(bool) * 4 * 4);
  memcpy(&PIECE_T.color, pieceTColor, sizeof(float) * 3);
}

// y, x, 1
int rotationCoefficients[4][3] = {
    {4, 1, 0}, {1, -4, 12}, {-4, -1, 15}, {-1, 4, 3}};

unsigned int rotatedIndex(unsigned int px, unsigned int py, unsigned int r) {
  int *coefficients = rotationCoefficients[r % 4];

  return coefficients[0] * py + coefficients[1] * px + coefficients[2] * 1;
}
