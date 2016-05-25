#include "helpers.hpp"
#include <iostream>
#include <SOIL/SOIL.h>

using namespace std;

void createPlane(float width, float length, GLuint &vao, GLuint &vbo,
    GLuint &ebo)
{
  float halfWidth = 0.5f * width;
  float halfLength = 0.5f * length;

  GLfloat vertices[] = {
    -halfWidth, 0.0f, +halfLength,
    +halfWidth, 0.0f, +halfLength,
    -halfWidth, 0.0f, -halfLength,
    +halfWidth, 0.0f, -halfLength
  };

  GLuint indices[] = { 0, 1, 2, 1, 3, 2 };

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, 
      GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 
      (GLvoid*)0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void createSkybox(float size, GLuint &vao, GLuint &vbo, GLuint &ebo, 
    GLuint &numIndices)
{
  float halfSize = size * 0.5f;

  GLfloat vertices[] = {
    -halfSize, -halfSize, +halfSize,
    +halfSize, -halfSize, +halfSize,
    +halfSize, +halfSize, +halfSize,
    -halfSize, +halfSize, +halfSize,
    -halfSize, -halfSize, -halfSize,
    +halfSize, -halfSize, -halfSize,
    +halfSize, +halfSize, -halfSize,
    -halfSize, +halfSize, -halfSize
  };

  GLuint indices[] = {
    0, 2, 1, 0, 3, 2, 
    0, 4, 7, 0, 7, 3,
    4, 5, 6, 4, 6, 7,
    1, 6, 5, 1, 2, 6,
    3, 7, 6, 3, 6, 2,
    0, 5, 4, 0, 1, 5
  };
  
  numIndices = 36;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, 
      GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 
      (GLvoid*)0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

GLenum glCheckError_(const char *file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
            case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
            case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
            case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
            case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
            case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
            case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cerr << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}

GLuint loadTexture(string filename) {
  int width, height;
  unsigned char *image = SOIL_load_image(filename.c_str(), &width, &height, 0, 
      SOIL_LOAD_RGB);

  if (image == nullptr) {
    std::cerr << "Cannot load texture \"" << filename << "\"." << std::endl;
    return -1;
  }

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, 
      GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);

  return texture;
}

GLuint loadCubemap(std::vector<std::string> faces) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char *image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for(GLuint i = 0; i < faces.size(); i++) {
		image = SOIL_load_image(faces[i].c_str(), &width, &height, 
			0, SOIL_LOAD_RGB);
    if (!image) cerr << "cannot load image " << faces[i] << "reason: " 
      << SOIL_last_result() << endl;
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
		);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}  
