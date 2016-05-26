#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec2 texCoord;

out VS_OUT {
  vec3 normal;
  vec3 tangent;
  vec2 texCoord;
  vec3 cameraDirection;
  vec3 lightDirection;
} vsOut;

uniform mat4 viewProj;
uniform mat4 modelMatrix;
uniform vec3 cameraPosition;
uniform vec3 lightPosition;

void main()
{
  vec4 worldPosition = modelMatrix * vec4(position, 1.0f);
  gl_Position = viewProj * worldPosition; 

  vsOut.cameraDirection = cameraPosition - worldPosition.xyz;
  vsOut.lightDirection = lightPosition - worldPosition.xyz;

  mat3 normalModelMatrix = mat3(modelMatrix);
  vsOut.normal = normalModelMatrix * normal;
  vsOut.tangent = normalModelMatrix * tangent;
  vsOut.texCoord = texCoord;
}
