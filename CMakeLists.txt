cmake_minimum_required(VERSION 3.20)
project(high-order-fd CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Eigen3 REQUIRED NO_MODULE)


add_executable(high-order main.cpp solver.cpp stencil.cpp)
target_link_libraries(high-order Eigen3::Eigen)
