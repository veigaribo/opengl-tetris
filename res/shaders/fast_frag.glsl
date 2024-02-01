#version 330 core

uniform sampler2D texture1;

in vec2 v_TexCoords;

layout(location = 0) out vec4 color;

void main() {
  color = texture(texture1, v_TexCoords);
};
