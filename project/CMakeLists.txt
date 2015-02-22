file(GLOB_RECURSE SRC_FILES *.cpp *.hpp)
add_executable(DemoScene ${SRC_FILES})
target_link_libraries(DemoScene ${ALL_LIBRARIES})
