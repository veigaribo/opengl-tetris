#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in uint colorIndex;

const vec4 colors[] = vec4[](
    vec4(0.0, 0.0, 0.0, 1), // Nothing
    vec4(1.0, 1.0, 1.0, 1), // Wall
    vec4(0.9, 0.9, 0.1, 1), // O
    vec4(0.9, 0.5, 0.1, 1), // L
    vec4(0.1, 0.1, 0.9, 1), // J
    vec4(0.2, 0.7, 0.9, 1), // I
    vec4(0.2, 0.9, 0.2, 1), // S
    vec4(0.9, 0.1, 0.1, 1), // Z
    vec4(0.8, 0.1, 0.8, 1) // T
);

out vec4 v_Color;

void main() {
  gl_Position = vec4(position.xy, 0, 1);
  v_Color = colors[colorIndex];
};
