// clang-format off
#include "GL/glew.h"
// clang-format on

#include "field.h"
#include "game.h"
#include "render.h"
#include <GL/gl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct RenderInfo {
  GLuint programId;
  GLuint vertexBufferId;
  GLuint vertexArrayId;
  GLuint indexBufferId;
  GLuint gameTextureId;

  GLint gameTextureLoc;
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

struct Vertex {
  // position
  float x;
  float y;

  // texture coordinates
  float texX;
  float texY;
};

struct Vertex vertexBuffer[] = {{.x = -1, .y = -1, .texX = 0, .texY = 0},
                                {.x = 1, .y = -1, .texX = 1, .texY = 0},
                                {.x = 1, .y = 1, .texX = 1, .texY = 1},
                                {.x = -1, .y = 1, .texX = 0, .texY = 1}};

unsigned int indexBuffer[] = {0, 1, 2, 2, 3, 0};

struct Texel {
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char alpha;
};

struct Texel gameTextureImage[FIELD_WIDTH * FIELD_HEIGHT];

void renderInit() {
#ifdef DEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

  glDebugMessageCallback(&handleGlDebugMessage, NULL);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                        GL_TRUE);
#endif

  const GLubyte *renderer = glGetString(GL_RENDERER); // get renderer string
  const GLubyte *version = glGetString(GL_VERSION);   // version as a string
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  GLuint vertexArrayId;
  glGenVertexArrays(1, &vertexArrayId);
  glBindVertexArray(vertexArrayId);
  renderInfo.vertexArrayId = vertexArrayId;

  GLuint vertexBufferId;
  glGenBuffers(1, &vertexBufferId);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
  glBufferData(GL_ARRAY_BUFFER, sizeof vertexBuffer, vertexBuffer,
               GL_STATIC_DRAW);
  renderInfo.vertexBufferId = vertexBufferId;

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex),
                        (const void *)offsetof(struct Vertex, x));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex),
                        (const void *)offsetof(struct Vertex, texX));

  GLuint indexBufferId;
  glGenBuffers(1, &indexBufferId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indexBuffer, indexBuffer,
               GL_STATIC_DRAW);
  renderInfo.indexBufferId = indexBufferId;

  GLuint gameTextureId;
  unsigned int textureUnit = 0;

  glActiveTexture(GL_TEXTURE0 + textureUnit);
  glGenTextures(1, &gameTextureId);
  glBindTexture(GL_TEXTURE_2D, gameTextureId);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FIELD_WIDTH, FIELD_HEIGHT, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, gameTextureImage);
  glGenerateMipmap(GL_TEXTURE_2D);
  renderInfo.gameTextureId = gameTextureId;

  GLuint programId = glCreateProgram();
  renderInfo.programId = programId;

  char *vertexShaderPath = "res/shaders/fast_vertex.glsl";
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

  char *fragShaderPath = "res/shaders/fast_frag.glsl";
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

  GLint gameTextureLoc = glGetUniformLocation(programId, "texture1");
  glUniform1i(gameTextureLoc, textureUnit);
  renderInfo.gameTextureLoc = gameTextureLoc;

  // Fill wall color just once since it never changes
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      unsigned int fieldIndex = to1D(x, y);

      bool isLeftWall = x == 0;
      bool isRightWall = x == FIELD_WIDTH - 1;
      bool isBottomWall = y == 0;

      if (isLeftWall || isRightWall || isBottomWall) {
        gameTextureImage[fieldIndex].red = 255;
        gameTextureImage[fieldIndex].green = 255;
        gameTextureImage[fieldIndex].blue = 255;
        gameTextureImage[fieldIndex].alpha = 255;
      };
    }
  }
}

#define COLOR(R, G, B)                                                         \
  gameTextureImage[i].red = (R);                                               \
  gameTextureImage[i].green = (G);                                             \
  gameTextureImage[i].blue = (B);                                              \
  gameTextureImage[i].alpha = 255

void render(struct GameState *game) {
  // Don't even check the walls
  for (size_t y = 1; y < FIELD_HEIGHT; ++y) {
    for (size_t x = 1; x < FIELD_WIDTH - 1; ++x) {
      size_t i = to1D(x, y);

      struct Tile *tile = &game->field[i];
      unsigned int value = tile->value;

      if (value == TILE_FREE) {
        COLOR(0, 0, 0);
      } else {
        unsigned int piece = GET_PIECE_TYPE(value);

        switch (piece) {
        case PIECE_TEMPLATE_O:
          COLOR(230, 230, 26);
          break;
        case PIECE_TEMPLATE_L:
          COLOR(230, 128, 26);
          break;
        case PIECE_TEMPLATE_J:
          COLOR(26, 26, 230);
          break;
        case PIECE_TEMPLATE_I:
          COLOR(51, 179, 230);
          break;
        case PIECE_TEMPLATE_S:
          COLOR(51, 230, 51);
          break;
        case PIECE_TEMPLATE_Z:
          COLOR(230, 26, 26);
          break;
        case PIECE_TEMPLATE_T:
          COLOR(204, 26, 204);
          break;
        }
      }
    }
  }

  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, FIELD_WIDTH, FIELD_HEIGHT, GL_RGBA,
                  GL_UNSIGNED_BYTE, gameTextureImage);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
}

void terminateRender() {
  glDeleteTextures(1, &renderInfo.gameTextureId);
  glDeleteVertexArrays(1, &renderInfo.vertexArrayId);
  glDeleteBuffers(1, &renderInfo.vertexBufferId);
  glDeleteBuffers(1, &renderInfo.indexBufferId);
  glDeleteProgram(renderInfo.programId);
}
