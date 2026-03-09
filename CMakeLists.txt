cmake_minimum_required(VERSION 3.20)
project(high-order-fd CXX)

# maybe C++20 is not the best choice
# we are not using advanced features
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Eigen3 REQUIRED NO_MODULE)


# the stencil library must be compiled separately
# as it's the only one dependent on the user option
set(STENCIL_LIB stencil)
add_library(${STENCIL_LIB} stencil.cpp)
target_link_libraries(${STENCIL_LIB} Eigen3::Eigen)
target_compile_definitions(${STENCIL_LIB} PUBLIC USE_EXACT_STENCILS)

add_executable(high-order main.cpp solver.cpp)
target_link_libraries(high-order Eigen3::Eigen ${STENCIL_LIB})
