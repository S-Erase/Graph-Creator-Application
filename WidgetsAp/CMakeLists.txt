cmake_minimum_required(VERSION 3.10)

# set the project name
project(WidgetsApp VERSION 0.9)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file(cConfig.h.in cConfig.h)

# add glm
add_subdirectory(glm::glm)

# add the executable
add_executable(WidgetsApp cApp.cpp)

#
target_link_libraries(WidgetsApp PUBLIC glm::glm)

# add the binary tree to the search path for include files
# so that we will find cConfig.h
target_include_directories(WidgetsApp PUBLIC "${PROJECT_BINARY_DIR}")