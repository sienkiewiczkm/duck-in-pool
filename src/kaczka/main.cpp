#include <algorithm>
#include <random>
#include <cstdlib>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#include "config.hpp"
#include "helpers.hpp"
#include "mesh.hpp"
#include "orbitingCamera.hpp"
#include "shaders.hpp"
#include "splines.hpp"
#include "waterSurface.hpp"

using namespace std;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

const GLuint WIDTH = 800, HEIGHT = 600;

OrbitingCamera camera;

int main()
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Kaczka", 
      nullptr, nullptr);
  glfwMakeContextCurrent(window);

  glfwSetKeyCallback(window, key_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glewExperimental = GL_TRUE;
  glewInit();

  GLuint woodTexture = loadTexture(ASSETS_PATH_PREFIX"textures/ducktex.jpg");

  int framebufferWidth, framebufferHeight;
  glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);  
  glViewport(0, 0, framebufferWidth, framebufferHeight);

  VertexShader vertexShader(SHADER_PATH_PREFIX"duck.vert");
  FragmentShader fragmentShader(SHADER_PATH_PREFIX"duck.frag");
  ShaderProgram program;
  program.attach(&vertexShader);
  program.attach(&fragmentShader);
  program.link();

  VertexShader cubeVertexShader(SHADER_PATH_PREFIX"cubemap.vert");
  FragmentShader cubeFragmentShader(SHADER_PATH_PREFIX"cubemap.frag");
  ShaderProgram cubeProgram;
  cubeProgram.attach(&cubeVertexShader);
  cubeProgram.attach(&cubeFragmentShader);
  cubeProgram.link();

  Mesh duck(ASSETS_PATH_PREFIX"meshes/duck.mesh");
  WaterSurface waterSurface;
  waterSurface.create(10.0f, 10.0f, 256, 256);

  double previousTime = glfwGetTime();
  double currentTime = glfwGetTime();
  double previousMousePositionX, previousMousePositionY;
  double currentMousePositionX, currentMousePositionY;
  double mouseSensitivityX = 0.1f;
  double mouseSensitivityY = 0.1f;

  glfwGetCursorPos(window, &currentMousePositionX, &currentMousePositionY);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  const float cDropTime = 0.05f;
  float dropSinceLastTime = 0.0f;

  random_device randomDevice;
  mt19937 generator(randomDevice());
  uniform_real_distribution<float> randomReal(-1, 1);
  uniform_real_distribution<float> randomDropPower(0.05f, 0.5f);

  GLuint cubeVAO, cubeVBO, cubeEBO, numIndices;
  createSkybox(10.0f, cubeVAO, cubeVBO, cubeEBO, numIndices);

	std::vector<std::string> cubeMapFilenames;
	cubeMapFilenames.push_back(ASSETS_PATH_PREFIX"textures/halftiles.png");
	cubeMapFilenames.push_back(ASSETS_PATH_PREFIX"textures/halftiles.png");
	cubeMapFilenames.push_back(ASSETS_PATH_PREFIX"textures/fullgraytiles.png");
	cubeMapFilenames.push_back(ASSETS_PATH_PREFIX"textures/fullbluetiles.png");
	cubeMapFilenames.push_back(ASSETS_PATH_PREFIX"textures/halftiles.png");
	cubeMapFilenames.push_back(ASSETS_PATH_PREFIX"textures/halftiles.png");
  loadTexture(cubeMapFilenames[0]);
	GLuint cubemap = loadCubemap(cubeMapFilenames);
  waterSurface.setCubemap(cubemap);

  BSpline2D spline;
  vector<glm::vec2> controlPoints;
  const int cDuckControlPoints = 10;
  for (auto i = 0; i < cDuckControlPoints; ++i) {
    float x = randomReal(generator) * 5.0f;
    float y = randomReal(generator) * 5.0f;
    controlPoints.push_back(glm::vec2(x, y));
  }
  spline.setLoopedControlPoints(controlPoints);

  camera.rotate(glm::radians(30.0f), glm::radians(45.0f));
  camera.setDist(7.0f);

  double duckParameter = 0.0f;
  while (!glfwWindowShouldClose(window))
  {
      previousTime = currentTime;
      currentTime = glfwGetTime();
      double deltaTime = currentTime - previousTime;
      duckParameter += deltaTime / 30.0f;
      duckParameter -= (int)(duckParameter);

      if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        duckParameter = 0.0f;
      }

      previousMousePositionX = currentMousePositionX;
      previousMousePositionY = currentMousePositionY;
      glfwGetCursorPos( window, &currentMousePositionX, 
          &currentMousePositionY
      );

      double mouseDeltaX = mouseSensitivityX * 
        (currentMousePositionX - previousMousePositionX);
      double mouseDeltaY = mouseSensitivityY * 
        (currentMousePositionY - previousMousePositionY);

      dropSinceLastTime += deltaTime;
      while (dropSinceLastTime > cDropTime) {
        auto dropX = 5.0f*randomReal(generator);
        auto dropZ = 5.0f*randomReal(generator);
        waterSurface.applyDisturbaceInWorldSpace(glm::vec3(dropX, 0, dropZ), 
            randomDropPower(generator));
        dropSinceLastTime -= cDropTime;
      }
      
      waterSurface.update(deltaTime);
      
      glfwPollEvents();
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(program.getId());

      if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        camera.rotate(-mouseDeltaY, mouseDeltaX);
      }

      float duckScale = 0.005f;

      auto viewMatrix = camera.getViewMatrix();
      auto projMatrix = glm::perspective(glm::radians(90.0f), 
          (float)WIDTH/HEIGHT, 0.1f, 100.0f);

      auto splinePosition = spline.evaluate(duckParameter);
      auto splineDerivative = spline.derivative(duckParameter);
      auto duckPosition = glm::vec3(splinePosition.x, 0.0f, splinePosition.y);
      float rotation = atan2f(-splineDerivative.y, splineDerivative.x)
        + glm::pi<float>();
      waterSurface.applyDisturbaceInWorldSpace(duckPosition, 0.5f);
      
      auto modelMatrix = glm::translate(glm::mat4(1.0f), duckPosition); 
      modelMatrix = glm::scale(modelMatrix, 
          glm::vec3(duckScale, duckScale, duckScale));
      modelMatrix = glm::rotate(modelMatrix, rotation,
          glm::vec3(0.0f, 1.0f, 0.0f));
      
      auto viewProj = projMatrix * viewMatrix;

      GLuint transformLoc = glGetUniformLocation(program.getId(), "viewProj");
      GLuint modelMatrixLocation = glGetUniformLocation(program.getId(),
          "modelMatrix");

      glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(viewProj));
      glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE,
          glm::value_ptr(modelMatrix));

      glUniform3fv(
          glGetUniformLocation(program.getId(), "cameraPosition"),
          1, 
          glm::value_ptr(camera.getPosition())
      );

      glUniform3fv(
          glGetUniformLocation(program.getId(), "lightPosition"),
          1,
          glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f))
      );

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, woodTexture);
      duck.draw();

			glUseProgram(cubeProgram.getId());
      transformLoc = glGetUniformLocation(cubeProgram.getId(), "viewProj");
      modelMatrixLocation = glGetUniformLocation(cubeProgram.getId(),
          "modelMatrix");
      glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(viewProj));
      glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, 
          glm::value_ptr(glm::mat4(1.0f)));
      glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
      glBindVertexArray(cubeVAO);
      glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);

      waterSurface.draw(viewProj, camera.getPosition());

      glfwSwapBuffers(window);
  }

  glfwTerminate();
  return 0;
}

void key_callback(
    GLFWwindow* window, 
    int key, 
    int scancode, 
    int action, 
    int mode
    )
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    float distDelta = 0.05f * yoffset;
    camera.setDist(max(1.0f, camera.getDist() + distDelta));
}

