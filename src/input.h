#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>

enum Input {
  INPUT_NONE = 0,
  INPUT_LEFT = 1,
  INPUT_RIGHT = 2,
  INPUT_DOWN = 3,
  INPUT_ROTATE = 4,
};

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
                 int mods);

enum Input getInput(GLFWwindow *window);

#endif /* INPUT_H */
