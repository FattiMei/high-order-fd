cmake_minimum_required(VERSION 3.20)
project(high-order-fd CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost 1.83.0 REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)


add_executable(high-order main.cpp)
target_link_libraries(high-order Eigen3::Eigen)


add_executable(test_boost_mutation test_boost_mutation.cpp)

add_executable(test_stencils test_stencils.cpp)
target_link_libraries(test_stencils Eigen3::Eigen)
