#version 330 core

in VS_OUT {
  vec3 position;
} fsIn;

out vec4 color;

uniform sampler2D cubeFaceTexture;

void main() {
  vec3 albedo = texture(cubeFaceTexture, fsIn.texCoord);
  color = albedo;
}
