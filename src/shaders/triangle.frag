#version 330 core

in vec3 outColor;
in vec2 outTexCoord;
out vec4 color;

uniform sampler2D textureSampler;
uniform mat4 viewProj;

void main()
{
  color = texture(textureSampler, outTexCoord);
} 
