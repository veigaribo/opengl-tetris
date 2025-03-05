// clang-format off
#include <GL/glew.h>
// clang-format on

#include "config.h"
#include "game.h"
#include "input.h"
#include "render.h"
#include "stdio.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <time.h>

#ifdef TRACK_FPS
#include "fps.h"
#endif

const float UPDATE_INTERVAL = 1.0 / UPDATE_FREQUENCY;
static float updateElapsed = 0; // Time since the last update

void updateInc(float dt) { updateElapsed += dt; }

bool shouldUpdate(float dt) {
  if (updateElapsed >= UPDATE_INTERVAL) {
    updateElapsed -= UPDATE_INTERVAL;
    return true;
  }

  return false;
}

int main() {
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef DEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(400, 600, "Hello World", NULL, NULL);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
  glfwSetKeyCallback(window, keyCallback);

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

#ifdef TRACK_FPS
  glfwSwapInterval(0);
#else
  glfwSwapInterval(1);
#endif

  glewInit();

  // init rng
  srand(time(0));

  struct GameState game;
  start(window, &game);
  renderInit();

  // used to measure dt
  double updateStartTime, updateEndTime;
  updateStartTime = glfwGetTime();

  float dt;

#ifdef TRACK_FPS
  float fps = 0;

  // used to measure fps
  double fpsStartTime, fpsEndTime;
#endif

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window)) {
    updateEndTime = glfwGetTime();

#ifdef TRACK_FPS
    fpsStartTime = updateEndTime;
#endif

    dt = updateEndTime - updateStartTime;

    updateInc(dt);
    while (shouldUpdate(dt)) {
      update(&game, UPDATE_INTERVAL);
    }

    updateStartTime = glfwGetTime();

    render(&game);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();

#ifdef TRACK_FPS
    fpsEndTime = glfwGetTime();
    dt = fpsEndTime - fpsStartTime;

    if (trackFps(&fps, dt)) {
      printf("FPS: %02.0f\n", fps);
    }
#endif
  }

  end(&game);
  terminateRender();
  glfwTerminate();

  return 0;
}
