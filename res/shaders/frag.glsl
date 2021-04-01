#version 330 core

in vec3 v_Color;

layout(location = 0) out vec4 color;

void main() {
  color = vec4(v_Color.rgb, 1.0);
};
