cmake_minimum_required(VERSION 2.8)
project(kaczka)

find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(GLFW REQUIRED glfw3)

set(ASSETS_PATH_PREFIX ${PROJECT_SOURCE_DIR}/assets/)
set(SHADER_PATH_PREFIX ${PROJECT_SOURCE_DIR}/src/shaders/)

configure_file(
  ${PROJECT_SOURCE_DIR}/src/config.hpp.in 
  ${PROJECT_BINARY_DIR}/config.hpp
)

include_directories(
  ${PROJECT_BINARY_DIR}
  ${GLEW_INCLUDE_DIRS}
  ${GLFW_INCLUDE_DIRS} 
  ${GLM_INCLUDE_DIRS}
)
link_directories(${GLFW_LIBRARY_DIRS})

add_executable(${PROJECT_NAME} 
  src/kaczka/helpers.cpp
  src/kaczka/main.cpp
  src/kaczka/mesh.cpp
  src/kaczka/orbitingCamera.cpp
  src/kaczka/shaders.cpp
  src/kaczka/splines.cpp
  src/kaczka/waterSurface.cpp
)

target_link_libraries(${PROJECT_NAME} 
  ${GLFW_LIBRARIES} 
  ${GLEW_LIBRARIES} 
  "-framework OpenGL"
  "-lSOIL"
)

target_compile_features(${PROJECT_NAME} PRIVATE
  cxx_auto_type
  cxx_nullptr
  cxx_range_for
)

