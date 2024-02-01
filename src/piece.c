#include "piece.h"
#include "common.h"
#include "view.h"
#include <string.h>

struct PieceTemplate PIECE_O = {
    .type = PIECE_TEMPLATE_O,
    // clang-format off
    .map = {
      0, 0, 0, 0,
      0, 1, 1, 0,
      0, 1, 1, 0,
      0, 0, 0, 0
    }
    // clang-format on
};

struct PieceTemplate PIECE_L = {
    .type = PIECE_TEMPLATE_L,
    // clang-format off
    .map = {
      0, 0, 0, 0,
      0, 1, 1, 0,
      0, 0, 1, 0,
      0, 0, 1, 0
    }
    // clang-format on
};
struct PieceTemplate PIECE_J = {
    .type = PIECE_TEMPLATE_J,
    // clang-format off
    .map = {
      0, 0, 0, 0,
      0, 1, 1, 0,
      0, 1, 0, 0,
      0, 1, 0, 0
    }
    // clang-format on
};
struct PieceTemplate PIECE_I = {
    .type = PIECE_TEMPLATE_I,
    // clang-format off
    .map = {
      0, 0, 1, 0,
      0, 0, 1, 0,
      0, 0, 1, 0,
      0, 0, 1, 0
    }
    // clang-format on
};
struct PieceTemplate PIECE_S = {
    .type = PIECE_TEMPLATE_S,
    // clang-format off
    .map = {
      0, 1, 0, 0,
      0, 1, 1, 0,
      0, 0, 1, 0,
      0, 0, 0, 0
    }
    // clang-format on
};
struct PieceTemplate PIECE_Z = {
    .type = PIECE_TEMPLATE_Z,
    // clang-format off
    .map = {
      0, 0, 1, 0,
      0, 1, 1, 0,
      0, 1, 0, 0,
      0, 0, 0, 0
    }
    // clang-format on
};
struct PieceTemplate PIECE_T = {
    .type = PIECE_TEMPLATE_T,
    // clang-format off
    .map = {
      0, 0, 1, 0,
      0, 1, 1, 0,
      0, 0, 1, 0,
      0, 0, 0, 0
    }
    // clang-format on
};

// y, x, 1
int rotationCoefficients[4][3] = {
    {4, 1, 0}, {1, -4, 12}, {-4, -1, 15}, {-1, 4, 3}};

unsigned int rotatedIndex(unsigned int px, unsigned int py, unsigned int r) {
  int *coefficients = rotationCoefficients[r % 4];

  return coefficients[0] * py + coefficients[1] * px + coefficients[2] * 1;
}
