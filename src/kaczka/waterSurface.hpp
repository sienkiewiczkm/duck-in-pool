#ifndef __WATER_SURFACE_HPP__
#define __WATER_SURFACE_HPP__

#include "shaders.hpp"

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <vector>

class WaterSurface {
public:
  WaterSurface();
  virtual ~WaterSurface();

  void create(float planeWidth, float planeHeight, 
      int samplesTextureWidth, int samplesTextureHeight);
  void free();

  void applyDisturbaceInWorldSpace(glm::vec3 position, float strength);
  void update(float deltaTime);
  void draw(const glm::mat4 &viewProj, const glm::vec3 &cameraPosition);

  inline void setCubemap(GLuint cubemap) { _cubemap = cubemap; }
  inline GLuint getCubemap() { return _cubemap; }

  inline glm::mat4 getModelMatrix() { return _modelMatrix; }
  inline void setModelMatrix(glm::mat4 modelMatrix) { 
    _modelMatrix = modelMatrix; 
    _invModelMatrix = glm::inverse(_modelMatrix);
  }

protected:
  void clearSamples();
  void calculateNormalMap();
  std::vector<GLubyte> buildNormalMap();
  void copyNormalsToTexture(std::vector<unsigned char> &normals);

  unsigned char convertNormalCoordToColor(float coord);

private:
  GLuint _vbo, _vao, _ebo;
  GLuint _cubemap;

  float _planeWidth, _planeHeight;
  int _samplesTextureWidth, _samplesTextureHeight;
  std::vector<float> _samples, _samples2;
  std::vector<glm::vec3> _normals;
  GLuint _normalMapTexture;

  std::vector<float> *_currentSamples, *_previousSamples;

  glm::mat4 _modelMatrix;
  glm::mat4 _invModelMatrix;
  glm::mat4 _textureMatrix;

  ShaderProgram _shader;
};

#endif
