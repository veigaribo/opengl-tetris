#include "input.h"
#include "common.h"
#include <stdio.h>

bool left = false;
bool right = false;
bool down = false;
bool rotate = false;

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods) {
  if (action == GLFW_PRESS) {

    if (key == GLFW_KEY_A || key == GLFW_KEY_H || key == GLFW_KEY_LEFT) {
      left = true;
    }

    if (key == GLFW_KEY_D || key == GLFW_KEY_L || key == GLFW_KEY_RIGHT) {
      right = true;
    }

    if (key == GLFW_KEY_S || key == GLFW_KEY_J || key == GLFW_KEY_DOWN) {
      down = true;
    }

    if (key == GLFW_KEY_Z || key == GLFW_KEY_SPACE) {
      rotate = true;
    }
  }
}

enum Input getInput(GLFWwindow *window) {
  enum Input input;

  if (left && !right) {
    input = INPUT_LEFT;
  } else if (right && !left) {
    input = INPUT_RIGHT;
  } else if (down) {
    input = INPUT_DOWN;
  } else if (rotate) {
    input = INPUT_ROTATE;
  } else {
    input = INPUT_NONE;
  }

  left = right = down = rotate = false;
  return input;
}
