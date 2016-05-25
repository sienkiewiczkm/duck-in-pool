#ifndef __HELPERS_HPP__
#define __HELPERS_HPP__

#include <gl/glew.h>
#include <string>
#include <vector>

void createPlane(float width, float length, GLuint &vao, GLuint &vbo,
    GLuint &ebo);
void createSkybox(float size, GLuint &vao, GLuint &vbo, GLuint &ebo, 
    GLuint &numIndices);

GLuint loadTexture(std::string filename);
GLuint loadCubemap(std::vector<std::string> faces);

GLenum glCheckError_(const char *file, int line);
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

#endif
