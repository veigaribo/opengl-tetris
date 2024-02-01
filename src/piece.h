#ifndef PIECE_H
#define PIECE_H

#include "common.h"

enum {
  PIECE_TEMPLATE_O = 1 << 2,
  PIECE_TEMPLATE_L = 2 << 2,
  PIECE_TEMPLATE_J = 3 << 2,
  PIECE_TEMPLATE_I = 4 << 2,
  PIECE_TEMPLATE_S = 5 << 2,
  PIECE_TEMPLATE_Z = 6 << 2,
  PIECE_TEMPLATE_T = 7 << 2,
};

struct PieceTemplate {
  unsigned int type;
  unsigned char map[4 * 4];
};

struct ActivePiece {
  unsigned int x;
  unsigned int y;
  unsigned int rotation;

  struct PieceTemplate *pieceTemplate;
};

extern struct PieceTemplate PIECE_O;
extern struct PieceTemplate PIECE_L;
extern struct PieceTemplate PIECE_J;
extern struct PieceTemplate PIECE_I;
extern struct PieceTemplate PIECE_S;
extern struct PieceTemplate PIECE_Z;
extern struct PieceTemplate PIECE_T;

// https://youtu.be/8OK8_tHeCIA?t=209
unsigned int rotatedIndex(unsigned int px, unsigned int py, unsigned int r);

#endif /* PIECE_H */
