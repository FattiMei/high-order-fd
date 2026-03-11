cmake_minimum_required(VERSION 3.20)
project(high-order-fd CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Eigen3 REQUIRED NO_MODULE)


option(USE_EXACT_STENCILS
       "use exact arithmetic (rational numbers) for computing the stencils"
       OFF)

if (USE_EXACT_STENCILS)
	find_package(Boost 1.83.0 REQUIRED)

	if(NOT Boost_FOUND)
		message("Could not find Boost, defaulting to inaccurate stencils")
		set(USE_EXACT_STENCILS OFF)
	endif()
endif()


include_directories(include/)


set(STENCIL_LIB stencil)
add_library(${STENCIL_LIB} src/stencil.cpp)
if (USE_EXACT_STENCILS)
	target_compile_definitions(${STENCIL_LIB} PUBLIC "USE_EXACT_STENCILS")
endif()
target_link_libraries(${STENCIL_LIB} Eigen3::Eigen)


add_executable(high-order src/main.cpp
                          src/problem.cpp
                          src/solvers/sparse.cpp)
target_link_libraries(high-order Eigen3::Eigen ${STENCIL_LIB})
