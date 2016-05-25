#version 330 core

in vec3 outPosition;
out vec4 color;

uniform samplerCube textureSampler;

void main()
{
  color = texture(textureSampler, outPosition);
} 
