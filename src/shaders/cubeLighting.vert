#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 viewProjMatrix;
uniform mat4 textureMatrix;

out VS_OUT {
  vec3 position;
} vsOut;

void main() {
  gl_Position = viewProjMatrix * position;
  vsOut.position = position;
}
