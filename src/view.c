// clang-format off
#include "GL/glew.h"
// clang-format on

#include "view.h"
#include "field.h"
#include "game.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct RenderInfo {
  GLuint programId;
  GLuint vertexBufferId;
  GLuint vertexArrayId;
  GLuint indexBufferId;
} renderInfo;

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

enum { FIELD_SIZE = FIELD_WIDTH * FIELD_HEIGHT };

// 1 quad for every tile
struct Vertex vertexBuffer[FIELD_SIZE * 4];

// A lot
// In practice will certainly use less
unsigned int indexBuffer[FIELD_SIZE * 6] = {0};

void fancyRenderInit() {
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

  glDebugMessageCallback(&handleGlDebugMessage, NULL);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                        GL_TRUE);

  const GLubyte *renderer = glGetString(GL_RENDERER); // get renderer string
  const GLubyte *version = glGetString(GL_VERSION);   // version as a string
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  GLuint vertexArrayId;
  glGenVertexArrays(1, &vertexArrayId);
  glBindVertexArray(vertexArrayId);
  renderInfo.vertexArrayId = vertexArrayId;

  // Assumes height will be bigger than width
  // float horizontalProportion = (float)FIELD_WIDTH / FIELD_HEIGHT;
  // float horizontalOffset = 0;

  // 0 - 1
  float blockHeight = 1.0 / FIELD_HEIGHT;
  float blockWidth = 1.0 / FIELD_WIDTH;

  // -1 - +1
  float scaledBlockHeight = blockHeight * 2;
  float scaledBlockWidth = blockWidth * 2;

  // Insert 1 quad for every tile
  for (unsigned int x = 0; x < FIELD_WIDTH; ++x) {
    for (unsigned int y = 0; y < FIELD_HEIGHT; ++y) {
      float quadX = x * scaledBlockWidth - 1;
      float quadY = y * scaledBlockHeight - 1;

      unsigned int vertexI = to1D(x, y) * 4;

      // Bottom left
      vertexBuffer[vertexI].x = quadX;
      vertexBuffer[vertexI].y = quadY;

      // Bottom right
      vertexBuffer[vertexI + 1].x = quadX + scaledBlockWidth;
      vertexBuffer[vertexI + 1].y = quadY;

      // Top right
      vertexBuffer[vertexI + 2].x = quadX + scaledBlockWidth;
      vertexBuffer[vertexI + 2].y = quadY + scaledBlockHeight;

      // Top left
      vertexBuffer[vertexI + 3].x = quadX;
      vertexBuffer[vertexI + 3].y = quadY + scaledBlockHeight;
    }
  }

  GLuint vertexBufferId;
  glGenBuffers(1, &vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, sizeof vertexBuffer, vertexBuffer,
               GL_DYNAMIC_DRAW);
  renderInfo.vertexBufferId = vertexBufferId;

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex),
                        (const void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, sizeof(struct Vertex),
                         (const void *)offsetof(struct Vertex, color));

  GLuint indexBufferId;
  glGenBuffers(1, &indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indexBuffer, indexBuffer,
               GL_DYNAMIC_DRAW);
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
}

void fancyRender(struct GameState *game) {
  glClear(GL_COLOR_BUFFER_BIT);

  size_t indexBufferIndex = 0;
  size_t vertexCount = 0;

  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      unsigned int fieldPosition = to1D(x, y);
      struct Tile tile = game->field[fieldPosition];

      unsigned int tileValue = tile.value;

      if (tileValue == 0) {
        continue;
      }

      unsigned char colorIndex;

      if (IS_OCCUPIED_BY_ACTIVE(tileValue) ||
          IS_OCCUPIED_BY_STATIC(tileValue)) {
        unsigned int pieceType = GET_PIECE_TYPE(tileValue);
        colorIndex = (pieceType >> 2) + 1;
      } else {
        colorIndex = 1;
      }

      size_t vertexIndex = fieldPosition * 4;
      struct Vertex *quad = &vertexBuffer[vertexIndex];

      quad[0].color = colorIndex;
      quad[1].color = colorIndex;
      quad[2].color = colorIndex;
      quad[3].color = colorIndex;

      indexBuffer[indexBufferIndex++] = vertexIndex + 0;
      indexBuffer[indexBufferIndex++] = vertexIndex + 1;
      indexBuffer[indexBufferIndex++] = vertexIndex + 2;
      indexBuffer[indexBufferIndex++] = vertexIndex + 2;
      indexBuffer[indexBufferIndex++] = vertexIndex + 3;
      indexBuffer[indexBufferIndex++] = vertexIndex + 0;

      vertexCount += 6;
    }
  }

  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof vertexBuffer, vertexBuffer);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof indexBuffer, indexBuffer);
  glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, NULL);
}

void terminateRender() {
  glDeleteBuffers(1, &renderInfo.vertexBufferId);
  glDeleteBuffers(1, &renderInfo.indexBufferId);
  glDeleteVertexArrays(1, &renderInfo.vertexArrayId);
  glDeleteProgram(renderInfo.programId);
}
