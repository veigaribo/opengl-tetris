// clang-format off
#include <GL/glew.h>
// clang-format on

#include "game.h"
#include "stdio.h"
#include "view.h"
#include "piece.h"
#include <stdlib.h>
#include <time.h>
#include <GLFW/glfw3.h>

int main() {
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(400, 600, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  glfwSwapInterval(1);

  glewInit();

  // init rng
  srand(time(0));

  initPieces();
  struct GameState *game = start(window);
  renderInit();

  // used to measure dt
  double startTime, endTime;
  startTime = glfwGetTime();

  float dt;

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    endTime = glfwGetTime();

    dt = endTime - startTime;

    update(game, dt);

    startTime = glfwGetTime();

    render(game);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  end(game);
  terminateRender();
  glfwTerminate();

  return 0;
}
