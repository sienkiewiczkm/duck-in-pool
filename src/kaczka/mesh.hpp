#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 texCoord;
};

class Mesh {
public:
  Mesh();
  Mesh(const std::string &filename);
  virtual ~Mesh();

  void load(const std::string &filename);
  void free();
  void draw();

  inline GLuint getVBO() { return _vbo; }

  inline int getNumVertices() { return _numVertices; }
  inline int getNumIndices() { return _numIndices; }
  inline int getNumTriangles() { return _numTriangles; }

private:  
  int _numVertices, _numTriangles, _numIndices;
  GLuint _vbo, _vao, _ebo;
};        
          
#endif    
