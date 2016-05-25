#version 330 core

layout (location = 0) in vec3 position;

out vec2 psTexCoord;
out vec3 psLightVec;
out vec3 psCameraVec;

uniform vec3 cameraPosition;
uniform mat4 viewProj;
uniform mat4 textureMatrix;

void main()
{
  gl_Position = viewProj * vec4(position, 1.0f);
  psTexCoord = (textureMatrix * vec4(position, 1.0f)).xz;
  psLightVec = vec3(0,5,0) - position;
  psCameraVec = cameraPosition - position;
}
