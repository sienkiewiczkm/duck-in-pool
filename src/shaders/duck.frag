#version 330 core

in vec3 outColor;
in vec2 outTexCoord;
out vec4 color;

uniform sampler2D textureSampler;
uniform mat4 viewProj;

in VS_OUT {
  vec3 normal;
  vec3 tangent;
  vec2 texCoord;
  vec3 cameraDirection;
  vec3 lightDirection;
} fsIn;

void main()
{
  vec3 normal = normalize(fsIn.normal);
  vec3 tangent = normalize(fsIn.tangent);
  vec3 cameraDirection = normalize(fsIn.cameraDirection);
  vec3 lightDirection = normalize(fsIn.lightDirection);
  vec3 lightReflection = reflect(-lightDirection, normal);

  float LN = dot(lightDirection, normal);
  float LT = dot(lightDirection, tangent);
  float VT = dot(cameraDirection, tangent);

  float NL = sqrt(1.0 - LT*LT);
  float VR = NL*sqrt(1.0 - VT*VT) - LT*VT;
  
  float IclassicDiffuse = clamp(LN, 0.0, 1.0);
  float Iambient = 0.4;
  float Idiffuse = 0.6 * clamp(NL, 0.0, 1.0);
  float Ispecular = 0.0;
  if (LN > 0.0) {
    Ispecular = pow(max(VR, 0.0f), 32);
  }

  vec4 tex = texture(textureSampler, fsIn.texCoord);
  vec4 ambient = tex;
  vec4 diffuse = IclassicDiffuse * tex;
  vec4 specular = 0.4 * Ispecular * vec4(1, 1, 1, 1);

  color = Iambient * ambient + IclassicDiffuse * (diffuse + specular);
} 
