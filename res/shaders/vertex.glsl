#version 330 core

layout(location = 0) in vec4 position;

out vec3 v_Color;

uniform vec2 u_Offset;
uniform vec3 u_Color;

void main() {
  gl_Position = vec4(position.xy + u_Offset.xy, 0, 1);

  v_Color = u_Color;
};
