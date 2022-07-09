cmake_minimum_required(VERSION 3.5)

project(Remap360Image LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV 4.5 REQUIRED)

add_executable(Remap360Image main.cpp simpleremap360.cpp simpleremap360.h)
target_link_libraries(Remap360Image ${OpenCV_LIBS})
