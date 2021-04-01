#pragma once

#include "common.h"

struct PieceTemplate {
  bool map[4 * 4];
  float color[3];
};

struct ActivePiece {
  unsigned int x;
  unsigned int y;
  unsigned int rotation;

  struct PieceTemplate pieceTemplate;
};

void initPieces();

extern struct PieceTemplate PIECE_O, PIECE_L, PIECE_J, PIECE_I, PIECE_S,
    PIECE_Z, PIECE_T;

// https://youtu.be/8OK8_tHeCIA?t=209
unsigned int rotatedIndex(unsigned int px, unsigned int py, unsigned int r);
