#include "waterSurface.hpp"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "config.hpp"
#include "helpers.hpp"

using namespace std;

WaterSurface::WaterSurface() : 
  _vbo(0), _vao(0), _ebo(0),
  _modelMatrix(1.0f), _planeWidth(0.0f),
  _planeHeight(0.0f), _samplesTextureWidth(0), _samplesTextureHeight(0),
  _normalMapTexture(0) {
    _invModelMatrix = glm::inverse(_modelMatrix);
}

WaterSurface::~WaterSurface() {
  free();
}

void WaterSurface::create(float planeWidth, float planeHeight,
    int samplesTextureWidth, int samplesTextureHeight) {
  _planeWidth = planeWidth;
  _planeHeight = planeHeight;
  _samplesTextureWidth = samplesTextureWidth;
  _samplesTextureHeight = samplesTextureHeight;

  clearSamples();
  auto normalMapData = buildNormalMap();

  glGenTextures(1, &_normalMapTexture);
  glBindTexture(GL_TEXTURE_2D, _normalMapTexture);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _samplesTextureWidth,
      _samplesTextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, &normalMapData[0]);
  glBindTexture(GL_TEXTURE_2D, 0);

  createPlane(_planeWidth, _planeHeight, _vbo, _vao, _ebo);

  _textureMatrix = glm::scale(glm::mat4(1.0f), 
      glm::vec3(1.0f/_planeWidth, 0, 1.0f/_planeHeight));
  _textureMatrix = glm::translate(_textureMatrix, 
      glm::vec3(0.5f * _planeWidth, 0, 0.5f * _planeHeight));

  VertexShader vertexShader(SHADER_PATH_PREFIX"water.vert");
  FragmentShader fragmentShader(SHADER_PATH_PREFIX"water.frag");
  _shader.attach(&vertexShader);
  _shader.attach(&fragmentShader);
  _shader.link();
}

void WaterSurface::free() {
  // todo: Free allocated resources.
}

void WaterSurface::applyDisturbaceInWorldSpace(glm::vec3 position, 
    float strength) {
  auto textureSpacePosition = glm::vec3(
      _textureMatrix * _invModelMatrix * glm::vec4(position, 1.0f)
  );
  
  if (textureSpacePosition.x < 0.0f || textureSpacePosition.x > 1.0f ||
      textureSpacePosition.z < 0.0f || textureSpacePosition.z > 1.0f) {
    return;
  }

  int coordX = (int)(textureSpacePosition.x * _samplesTextureWidth);
  int coordY = (int)(textureSpacePosition.z * _samplesTextureHeight);
  (*_currentSamples)[coordY*_samplesTextureHeight+coordX] += strength;
}

void WaterSurface::update(float deltaTime) {
  int N = 256;
  float h = 2.0f / (N-1);
  float c = 1.0f;
  float dt = 1.0f / N;

  float A = (c*c)*(dt*dt)/(h*h);
  float B = 2 - 4*A;
  
  for (auto y = 0; y < _samplesTextureHeight; ++y) {
    for (auto x = 0; x < _samplesTextureWidth; ++x) {
      auto baseIndex = y * _samplesTextureWidth + x;
      float previous = (*_previousSamples)[baseIndex];
      float current = (*_currentSamples)[baseIndex];
      float neighborsSum = 0.0f;
      int dispX[] = { -1, 1,  0, 0 };
      int dispY[] = {  0, 0, -1, 1 };
      for (int i = 0; i < 4; ++i) {
        auto fx = x + dispX[i];
        auto fy = y + dispY[i];
        if (fx >= 0 && fx < _samplesTextureWidth &&
            fy >= 0 && fy < _samplesTextureHeight) {
          neighborsSum += (*_currentSamples)[fy*_samplesTextureWidth+fx];
        }
      }

      float px = ((float)x) / (_samplesTextureWidth-1);
      float py = ((float)y) / (_samplesTextureHeight-1);
      float l = min(px, min(1.0f - px, min(py, 1.0f - py)));
      float damping = 0.95f * min(1.0f, l/0.01f);

      (*_previousSamples)[baseIndex] 
        = damping * (A*neighborsSum + B*current - previous);
    }
  }

  swap(_currentSamples, _previousSamples);
  calculateNormalMap();
}

void WaterSurface::draw(const glm::mat4 &viewProj, 
    const glm::vec3 &cameraPosition) {
  auto normalMapData = buildNormalMap();
  copyNormalsToTexture(normalMapData);
  
  glUseProgram(_shader.getId());

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _normalMapTexture);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _cubemap);

  glUniform1i(glGetUniformLocation(_shader.getId(), "textureSampler"), 0);
  glUniform1i(glGetUniformLocation(_shader.getId(), "cubemapSampler"), 1);

  GLuint viewMatrix = glGetUniformLocation(_shader.getId(), "viewProj");
  GLuint textureMatrixLoc = 
    glGetUniformLocation(_shader.getId(), "textureMatrix");
  GLuint cameraPosLoc = glGetUniformLocation(_shader.getId(), "cameraPosition");

  glUniformMatrix4fv(textureMatrixLoc, 1, GL_FALSE, 
      glm::value_ptr(_textureMatrix));
  glUniformMatrix4fv(viewMatrix, 1, GL_FALSE, glm::value_ptr(viewProj));
  glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPosition));

  glBindVertexArray(_vbo);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void WaterSurface::clearSamples() {
  auto totalSamples = _samplesTextureWidth * _samplesTextureHeight;
  _samples.resize(totalSamples);
  _samples2.resize(totalSamples);
  _normals.resize(totalSamples);

  _currentSamples = &_samples;
  _previousSamples = &_samples2;

  for (auto i = 0; i < totalSamples; ++i) {
    _samples[i] = 0.0f;
    _samples2[i] = 0.0f;
    _normals[i] = glm::vec3(0.0f, 1.0f, 0.0f);
  }
}

void WaterSurface::calculateNormalMap() {
  for (auto y = 0; y < _samplesTextureHeight; ++y) {
    for (auto x = 0; x < _samplesTextureWidth; ++x) {
      auto baseIndex = y * _samplesTextureWidth + x;
      _normals[baseIndex] = glm::vec3(0.0f, 0.0f, 0.0f);
    }
  }
   
  for (auto y = 0; y < _samplesTextureHeight; ++y) {
    for (auto x = 0; x < _samplesTextureWidth; ++x) {
      auto baseIndex = y * _samplesTextureWidth + x;

      int dispX[] = { -1, 1 };
      int dispY[] = { -1, 1 };
      for (auto i = 0; i < 2; ++i) {
        auto fx = x + dispX[i];
        auto fy = y + dispY[i];
        if (fx < 0 || fx >= _samplesTextureHeight ||
            fy < 0 || fy >= _samplesTextureWidth) {
          continue;
        }

        auto dxShiftIndex = y * _samplesTextureWidth + fx;
        auto dyShiftIndex = fy * _samplesTextureWidth + x;

        glm::vec3 originPosition(x, (*_currentSamples)[baseIndex], y);
        glm::vec3 dxPosition(fx, (*_currentSamples)[dxShiftIndex], y);
        glm::vec3 dyPosition(x, (*_currentSamples)[dyShiftIndex], fy);

        auto dx = dxPosition - originPosition;
        auto dy = dyPosition - originPosition;
        _normals[baseIndex] += glm::cross(dy, dx);
      }

      _normals[baseIndex] = glm::normalize(_normals[baseIndex]);
    }
  }
}

vector<GLubyte> WaterSurface::buildNormalMap() {
  // todo: Read about move constructor.
  auto totalSamples = _samplesTextureWidth * _samplesTextureHeight;
  vector<unsigned char> normalMapData(3*totalSamples);

  for (auto y = 0; y < _samplesTextureHeight; ++y) {
    for (auto x = 0; x < _samplesTextureWidth; ++x) {
      auto baseIndex = y * _samplesTextureWidth + x;
      auto normal = _normals[baseIndex];
      for (auto i = 0; i < 3; ++i) {
        normalMapData[3*baseIndex+i] = convertNormalCoordToColor(normal[i]);
      }
    }
  }

  return normalMapData;
}

void WaterSurface::copyNormalsToTexture(vector<unsigned char> &normals) {
  glBindTexture(GL_TEXTURE_2D, _normalMapTexture);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _samplesTextureWidth,
      _samplesTextureHeight, GL_RGB, GL_UNSIGNED_BYTE, &normals[0]);
  glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned char WaterSurface::convertNormalCoordToColor(float coord) {
  coord = max(-1.0f, min(1.0f, coord));
  return (unsigned char)(255.0f * (coord + 1.0f) / 2.0f);
}
