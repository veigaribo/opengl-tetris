// clang-format off
#include "GL/glew.h"
// clang-format on

#include "view.h"
#include "field.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct RenderInfo {
  GLuint programId;
  GLuint vertexBufferId;
  GLuint vertexArrayId;
  GLuint indexBufferId;

  GLuint uOffsetLocation;
  GLuint uColorLocation;
} renderInfo;

const unsigned int QUAD_VERTEX_INDICES[] = {0, 1, 2, 2, 3, 0};

void APIENTRY handleGlDebugMessage(GLenum source, GLenum type, GLuint id,
                                   GLenum severity, GLsizei length,
                                   const GLchar *message,
                                   const void *userParam) {
  char ntMessage[length + 1];
  memcpy(&ntMessage, message, length);
  ntMessage[length] = 0;

  printf("[OpenGL Debug Message] source: %d, type: %d, id: %d, severity: %d"
         "message: %s\n",
         source, type, id, severity, ntMessage);
}

void renderInit() {
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

  glDebugMessageCallback(&handleGlDebugMessage, NULL);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                        GL_TRUE);

  const GLubyte *renderer = glGetString(GL_RENDERER); // get renderer string
  const GLubyte *version = glGetString(GL_VERSION);   // version as a string
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  float blockWidth = 1.0f / (FIELD_WIDTH * 2.0f);
  float blockHeight = 1.0f / (FIELD_HEIGHT * 2.0f);

  // clang-format off
  struct Vertex vertexBuffer[] = {
    -blockWidth, -blockHeight,
    blockWidth, -blockHeight,
    blockWidth, blockHeight,
    -blockWidth, blockHeight
  };
  // clang-format on

  GLuint vertexArrayId;
  glGenVertexArrays(1, &vertexArrayId);
  glBindVertexArray(vertexArrayId);
  renderInfo.vertexArrayId = vertexArrayId;

  GLuint vertexBufferId;
  glGenBuffers(1, &vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(struct Vertex), vertexBuffer,
               GL_STATIC_DRAW);
  renderInfo.vertexBufferId = vertexBufferId;

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);

  GLuint indexBufferId;
  glGenBuffers(1, &indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int),
               QUAD_VERTEX_INDICES, GL_STATIC_DRAW);
  renderInfo.indexBufferId = indexBufferId;

  GLuint programId = glCreateProgram();
  renderInfo.programId = programId;

  char *vertexShaderPath = "res/shaders/vertex.glsl";
  FILE *vertexShaderFile = fopen(vertexShaderPath, "r");

  fseek(vertexShaderFile, 0, SEEK_END);
  long vertexShaderLength = ftell(vertexShaderFile);
  fseek(vertexShaderFile, 0, SEEK_SET);

  // + 1 for null at the end
  // vertexShader1 is not `const`, vertexShader is
  char *const vertexShader1 = malloc(vertexShaderLength + 1);
  fread(vertexShader1, 1, vertexShaderLength, vertexShaderFile);

  vertexShader1[vertexShaderLength] = 0;

  fclose(vertexShaderFile);
  const char *const vertexShader = vertexShader1;

  unsigned int vertexShaderId = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertexShaderId, 1, &vertexShader, NULL);
  glCompileShader(vertexShaderId);

  free(vertexShader1);

  glAttachShader(programId, vertexShaderId);
  glDeleteShader(vertexShaderId);

  char *fragShaderPath = "res/shaders/frag.glsl";
  FILE *fragShaderFile = fopen(fragShaderPath, "r");

  fseek(fragShaderFile, 0, SEEK_END);
  long fragShaderLength = ftell(fragShaderFile);
  fseek(fragShaderFile, 0, SEEK_SET);

  // + 1 for null at the end
  // fragShader1 is not `const`, fragShader is
  char *const fragShader1 = malloc(fragShaderLength + 1);
  fread(fragShader1, 1, fragShaderLength, fragShaderFile);

  fragShader1[fragShaderLength] = 0;

  fclose(fragShaderFile);
  const char *const fragShader = fragShader1;

  unsigned int fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(fragShaderId, 1, &fragShader, NULL);
  glCompileShader(fragShaderId);

  free(fragShader1);

  glAttachShader(programId, fragShaderId);
  glDeleteShader(fragShaderId);

  glLinkProgram(programId);
  glValidateProgram(programId);
  glUseProgram(programId);

  GLint uOffsetLocation = glGetUniformLocation(programId, "u_Offset");
  renderInfo.uOffsetLocation = uOffsetLocation;
  GLint uColorLocation = glGetUniformLocation(programId, "u_Color");
  renderInfo.uColorLocation = uColorLocation;
}

void render(struct GameState *game) {
  glClear(GL_COLOR_BUFFER_BIT);

  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      unsigned int fieldPosition = to1D(x, y);
      struct Tile tile = game->field[fieldPosition];

      enum TileType tileType = tile.type;
      float *color = tile.color;

      float offsetX = (float)x / FIELD_WIDTH - 0.5f;
      float offsetY = (float)y / FIELD_HEIGHT - 0.5f;

      // not TILE_FREE
      if (tileType) {
        glUniform2f(renderInfo.uOffsetLocation, offsetX, offsetY);
        glUniform3f(renderInfo.uColorLocation, color[0], color[1], color[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
      }
    }
  }
}

void terminateRender() {
  glDeleteBuffers(1, &renderInfo.vertexBufferId);
  glDeleteBuffers(1, &renderInfo.indexBufferId);
  glDeleteVertexArrays(1, &renderInfo.vertexArrayId);
  glDeleteProgram(renderInfo.programId);
}
