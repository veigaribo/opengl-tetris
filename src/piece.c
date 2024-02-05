#include "piece.h"

// When using this, the bottom left number becomes index 0
// and it increases towards the top right
#define MAP(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14,   \
            A15)                                                               \
  { A12, A13, A14, A15, A8, A9, A10, A11, A4, A5, A6, A7, A0, A1, A2, A3 }
// See example below

struct PieceTemplate PIECE_O = {
    .type = PIECE_TEMPLATE_O,
    // clang-format off
    // Example

    .map = MAP(
// Column   0, 1, 2, 3
/* Row 3 */ 0, 0, 0, 0, // Index 12 -- 15
/* Row 2 */ 0, 1, 1, 0, // Index 8 -- 11
/* Row 1 */ 0, 1, 1, 0, // Index 4 -- 7
/* Row 0 */ 0, 0, 0, 0  // Index 0 -- 3
    )

    // clang-format on
};

struct PieceTemplate PIECE_L = {
    .type = PIECE_TEMPLATE_L,
    // clang-format off
    .map = MAP(
      0, 0, 0, 0,
      0, 1, 1, 0,
      0, 0, 1, 0,
      0, 0, 1, 0
    )
    // clang-format on
};
struct PieceTemplate PIECE_J = {
    .type = PIECE_TEMPLATE_J,
    // clang-format off
    .map = MAP(
      0, 0, 0, 0,
      0, 1, 1, 0,
      0, 1, 0, 0,
      0, 1, 0, 0
    )
    // clang-format on
};
struct PieceTemplate PIECE_I = {
    .type = PIECE_TEMPLATE_I,
    // clang-format off

    // Those 2 empty columns cause this piece's position to
    // sometimes extend past the walls into underflow land
    // But that doesn't seem to be a problem
    .map = MAP(
      0, 0, 1, 0,
      0, 0, 1, 0,
      0, 0, 1, 0,
      0, 0, 1, 0
    )
    // clang-format on
};
struct PieceTemplate PIECE_S = {
    .type = PIECE_TEMPLATE_S,
    // clang-format off
    .map = MAP(
      0, 1, 0, 0,
      0, 1, 1, 0,
      0, 0, 1, 0,
      0, 0, 0, 0
    )
    // clang-format on
};
struct PieceTemplate PIECE_Z = {
    .type = PIECE_TEMPLATE_Z,
    // clang-format off
    .map = MAP(
      0, 0, 1, 0,
      0, 1, 1, 0,
      0, 1, 0, 0,
      0, 0, 0, 0
    )
    // clang-format on
};
struct PieceTemplate PIECE_T = {
    .type = PIECE_TEMPLATE_T,
    // clang-format off
    .map = MAP(
      0, 0, 1, 0,
      0, 1, 1, 0,
      0, 0, 1, 0,
      0, 0, 0, 0
    )
    // clang-format on
};

// y, x, 1
int8_t rotationCoefficients[4][3] = {
    {4, 1, 0}, {1, -4, 12}, {-4, -1, 15}, {-1, 4, 3}};

// `r` must be between 0 and 3 inclusive
uint8_t rotatedIndex(uint8_t px, uint8_t py, uint8_t r) {
  int8_t *coefficients = rotationCoefficients[r];

  return coefficients[0] * py + coefficients[1] * px + coefficients[2] * 1;
}
