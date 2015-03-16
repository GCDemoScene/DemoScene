project(DemoScene)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-Wall -std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake_modules")

# Detect and add OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# Detect and add SFML
set(CMAKE_PREFIX_PATH "thirdParty/SFML-2.2")
find_package(SFML 2 REQUIRED system window graphics audio)
include_directories(${SFML_INCLUDE_DIR})

# Add GLEW
include_directories(thirdParty/glew-1.12.0/include)

# Add GLM
include_directories(thirdParty/glm)

set(ALL_LIBRARIES GLEW ${OPENGL_LIBRARIES} ${SFML_LIBRARIES})

include_directories(./project/include)
add_subdirectory(project)

# Copy ressources and shaders
file(COPY ./project/src/shaders/ DESTINATION project/src/shaders)
file(COPY ./project/resources/ DESTINATION project/resources)
