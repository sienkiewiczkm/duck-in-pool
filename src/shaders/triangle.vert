#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec3 outColor;
out vec2 outTexCoord;

uniform mat4 viewProj;
uniform mat4 modelMatrix;

void main()
{
  gl_Position = viewProj * modelMatrix * vec4(position, 1.0f);
  outColor = color;
  outTexCoord = vec2(texCoord.x, texCoord.y);
}
