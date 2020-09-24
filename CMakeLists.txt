CMAKE_MINIMUM_REQUIRED(VERSION 3.17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)

project(shadertool)

set(SPIRV_CROSS_STATIC ON)

add_subdirectory(3rd-party)

add_subdirectory(shadertool)
