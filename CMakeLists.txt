project(DemoScene)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_FLAGS "-Wall -std=c++11")
aux_source_directory(. SRC_LIST)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(SFML REQUIRED)
include_directories(${SFML_INCLUDE_DIR})

set(SFML_LIBRAIRIES sfml-graphics sfml-audio sfml-window sfml-system)

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(DemoScene ${SFML_LIBRAIRIES})


