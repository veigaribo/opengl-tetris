#include "common.h"

#define FRAMES_TO_UPDATE 6000
#define TIME_TO_UPDATE 1.0

float fpsUpdateTimeAccum = 0;

float fpsDtValues[FRAMES_TO_UPDATE] = {0};
size_t fpsFrameCount = 0;

static float average(float *items, size_t count) {
  float total = 0;

  for (size_t i = 0; i < count; ++i) {
    total += items[i];
  }

  return total / count;
}

// Update each 60 frames or 1s, whichever happens first
// Returns true if updated; false otherwise
bool trackFps(float *dest, float dt) {
  fpsDtValues[fpsFrameCount] = dt;
  fpsFrameCount = (fpsFrameCount + 1) % FRAMES_TO_UPDATE;

  fpsUpdateTimeAccum += dt;

  // 30 frames passed
  if (fpsFrameCount == 0) {
    *dest = 1 / average(fpsDtValues, FRAMES_TO_UPDATE);

    fpsUpdateTimeAccum = 0;
    return true;
  }

  // 0.5s passed
  if (fpsUpdateTimeAccum >= TIME_TO_UPDATE) {
    *dest = 1 / average(fpsDtValues, fpsFrameCount);

    fpsUpdateTimeAccum -= TIME_TO_UPDATE;
    fpsFrameCount = 0;

    return true;
  }

  return false;
}
