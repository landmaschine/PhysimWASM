#version 300 es
precision mediump float;

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 projection;

void main() {
  gl_Position = projection * model * vec4(aPos, 0.0, 1.0);
  TexCoord = aTexCoord;
}
