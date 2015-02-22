project(DemoScene)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-Wall -std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(SFML REQUIRED)
include_directories(${SFML_INCLUDE_DIR})

set(SFML_LIBRAIRIES sfml-graphics sfml-audio sfml-window sfml-system)

include_directories(thirdParty/glew-1.12.0/include)
add_definitions(-DGLEW_STATIC=1)
add_subdirectory(thirdParty/glew-1.12.0)

include_directories(thirdParty/glm)

set(ALL_LIBRARIES GLEW ${OPENGL_LIBRARIES} ${SFML_LIBRAIRIES})

include_directories(./project/include)
add_subdirectory(project)
