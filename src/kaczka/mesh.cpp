#include "mesh.hpp"
#include <fstream>
#include <iostream>

using namespace std;

Mesh::Mesh() : _vbo(0), _vao(0), _ebo(0) {
}

Mesh::Mesh(const string &filename) : Mesh() {
  load(filename);
}

Mesh::~Mesh() {
  free();
}

void Mesh::load(const string &filename) {
  // todo: Split this multipurpose method into smaller methods.
  // todo: Catch and handle exceptions.
  ifstream file;
  file.open(filename);

  file >> _numVertices;
  vector<VertexNormalTangentTex> vertices(_numVertices);

  for (auto i = 0; i < _numVertices; ++i) {
    file >> vertices[i].position.x 
      >> vertices[i].position.y 
      >> vertices[i].position.z
      >> vertices[i].normal.x
      >> vertices[i].normal.y
      >> vertices[i].normal.z
      >> vertices[i].texCoord.x
      >> vertices[i].texCoord.y;
  }

  calculateTangentVector(vertices);

  file >> _numTriangles;
  _numIndices = _numTriangles * 3;
  vector<GLuint> indices(_numIndices);

  for (auto i = 0; i < _numTriangles; ++i) {
    file >> indices[3*i] >> indices[3*i+1] >> indices[3*i+2];
  }

  glGenVertexArrays(1, &_vao);
  glGenBuffers(1, &_vbo);
  glGenBuffers(1, &_ebo);

  glBindVertexArray(_vao);
  glBindBuffer(GL_ARRAY_BUFFER, _vbo);
  glBufferData(GL_ARRAY_BUFFER, 
      vertices.size() * sizeof(VertexNormalTangentTex),
      &vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
      &indices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
      sizeof(VertexNormalTangentTex), 
      (GLvoid*)offsetof(VertexNormalTangentTex, position));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
      sizeof(VertexNormalTangentTex), 
      (GLvoid*)offsetof(VertexNormalTangentTex, normal));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 
      sizeof(VertexNormalTangentTex),
      (GLvoid*)offsetof(VertexNormalTangentTex, tangent));
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 
      sizeof(VertexNormalTangentTex),
      (GLvoid*)offsetof(VertexNormalTangentTex, texCoord));

  glBindVertexArray(0);
}

void Mesh::free() {
  // todo: Free allocated resources.
}

void Mesh::draw() {
  glBindVertexArray(_vbo);
  glDrawElements(GL_TRIANGLES, _numIndices, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Mesh::calculateTangentVector(
    std::vector<VertexNormalTangentTex> &vertices
) {
  glm::vec3 tangentPlaneNormal(0.0f, 1.0f, 0.0f);
  glm::vec3 idealTangentVector(1.0f, 0.0f, 0.0f);
  for (auto i = 0; i < vertices.size(); ++i) {
    glm::vec3 tangent;
    const auto &normal = vertices[i].normal;
    float cosa = glm::dot(tangentPlaneNormal, normal);
    if (fabs(cosa) > 0.95f) {
      tangent = idealTangentVector;
    } else {
      tangent = glm::cross(normal, tangentPlaneNormal);
    }
    
    vertices[i].tangent = tangent;
  }
}
