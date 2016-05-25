#version 330 core

in vec2 psTexCoord;
in vec3 psLightVec;
in vec3 psCameraVec;
out vec4 color;

uniform sampler2D textureSampler;
uniform samplerCube cubemapSampler;

vec3 cubemapCoordFromAnyPoint(vec3 origin, vec3 direction) {
  float t = min(max((1-origin.x)/direction.x, (-1-origin.x)/direction.x),
            min(max((1-origin.y)/direction.y, (-1-origin.y)/direction.y),
                max((1-origin.z)/direction.z, (-1-origin.z)/direction.z)));
  return origin + t * direction;
}

const float rzero = pow((1.33 - 1.0)/(1.33 + 1.0), 2.0);
float schlick(float cosfi) {
  return rzero + (1.0 - rzero)*pow(1.0 - cosfi, 5.0);
}

void main()
{
  vec3 lightColor = vec3(1, 1, 1);
  vec3 lightVec = normalize(psLightVec);
  vec3 cameraVec = normalize(psCameraVec);

  vec3 waterNormalTex = texture(textureSampler, psTexCoord).rgb;
  vec3 waterNormal = normalize(2.0 * (waterNormalTex - vec3(0.5, 0.5, 0.5)));

  float diffuse = clamp(dot(waterNormal, lightVec), 0.0f, 1.0f);
  vec3 diffuseColor = diffuse * lightColor;

  vec3 reflectVec = reflect(-lightVec, waterNormal);
  float specularStrength = 0.2f;
  float specular = pow(max(dot(cameraVec, reflectVec), 0.0), 32);
  vec3 specularColor = specularStrength * specular * lightColor;

  vec2 positionOnCubePlane = (2.0 * psTexCoord) - 1.0;
  vec3 positionInCube = vec3(positionOnCubePlane.x, 0, positionOnCubePlane.y);

  float exposure = 0.34;
  vec3 waterReflectionVec = reflect(-cameraVec, waterNormal);
  vec3 fixedWaterReflectionVec = cubemapCoordFromAnyPoint(positionInCube,
      waterReflectionVec);
  vec3 cubemapReflectionColor = exposure * texture(cubemapSampler,
      fixedWaterReflectionVec).rgb;

  float refractionRatio = 1.0 / 1.33;
  vec3 waterRefractionVec = refract(-cameraVec, waterNormal, refractionRatio);
  vec3 fixedWaterRefractionVec = cubemapCoordFromAnyPoint(positionInCube,
      waterRefractionVec);
  vec3 cubemapRefractionColor = exposure * texture(cubemapSampler, 
      fixedWaterRefractionVec).rgb;

  float fresnelFactor = schlick(abs(cameraVec.y));
  vec3 reflRefrFactor = (1.0 - fresnelFactor) * cubemapRefractionColor +
    fresnelFactor * cubemapReflectionColor;

  vec3 finalColor = reflRefrFactor * (diffuseColor + specularColor);
  color = vec4(finalColor, 1);
} 
