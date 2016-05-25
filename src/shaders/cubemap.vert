#version 330 core

layout (location = 0) in vec3 position;

out vec3 outPosition;

uniform mat4 viewProj;
uniform mat4 modelMatrix;

void main()
{
  gl_Position = viewProj * modelMatrix * vec4(position, 1.0f);
  outPosition = position;
}
