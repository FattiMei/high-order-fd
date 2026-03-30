cmake_minimum_required(VERSION 3.20)
project(high-order-fd CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Eigen3 REQUIRED NO_MODULE)
find_package(LAPACK REQUIRED)
find_package(Python3)

# this is only used to profile the memory throughput of the machine
find_package(likwid)


option(USE_EXACT_STENCILS
       "use exact arithmetic (rational numbers) for computing the stencils"
       OFF)

if (USE_EXACT_STENCILS)
	find_package(Boost)

	if(NOT Boost_FOUND)
		message("Could not find Boost, defaulting to inaccurate stencils")
		set(USE_EXACT_STENCILS OFF)
	endif()
endif()


include_directories(include/)
enable_testing()


set(STENCIL_LIB stencil)
add_library(${STENCIL_LIB} src/stencil.cpp)
if (USE_EXACT_STENCILS)
	target_compile_definitions(${STENCIL_LIB} PUBLIC "USE_EXACT_STENCILS")
endif()
target_link_libraries(${STENCIL_LIB} Eigen3::Eigen)


set(SOLVER_LIB solver)
set(SOLVER_DIR src/solvers)
file(GLOB SOLVER_SRC "${SOLVER_DIR}/*.cpp")
add_library(${SOLVER_LIB} ${SOLVER_SRC})
target_link_libraries(${SOLVER_LIB} Eigen3::Eigen LAPACK::LAPACK -llapacke)


set(HIGH_ORDER_EXE high-order)
set(WORK_ERROR_RESULTS "work_error.csv")
add_executable(${HIGH_ORDER_EXE} src/main.cpp src/problem.cpp)
target_link_libraries(${HIGH_ORDER_EXE} Eigen3::Eigen ${STENCIL_LIB} ${SOLVER_LIB})

add_custom_command(
	OUTPUT ${WORK_ERROR_RESULTS}
	COMMAND ${HIGH_ORDER_EXE} > ${WORK_ERROR_RESULTS}
	DEPENDS ${HIGH_ORDER_EXE}
)

add_custom_target(pareto
	COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/pareto.py ${WORK_ERROR_RESULTS}
	DEPENDS ${WORK_ERROR_RESULTS}
)


add_executable(test_matrix_assembly tests/test_matrix_assembly.cpp)
target_link_libraries(test_matrix_assembly Eigen3::Eigen ${SOLVER_LIB})
add_test(NAME test_matrix_assembly COMMAND ./test_matrix_assembly)


set(BENCH_ASSEMBLY_EXE bench_matrix_assembly)
set(BENCH_ASSEMBLY_RESULTS "bench_assembly.csv")
set(BENCH_MEMORY_THROUGHPUT_RESULTS "bench_throughput.csv")
set(BENCH_MEMORY_THROUGHPUT_COMMAND "${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/likwid_bench.py store --num_threads 1")
add_executable(${BENCH_ASSEMBLY_EXE} tests/bench_matrix_assembly.cpp)
target_link_libraries(${BENCH_ASSEMBLY_EXE} Eigen3::Eigen ${SOLVER_LIB})

add_custom_command(
	OUTPUT ${BENCH_ASSEMBLY_RESULTS}
	COMMAND ${BENCH_ASSEMBLY_EXE} 1000000 > ${BENCH_ASSEMBLY_RESULTS}
	DEPENDS ${BENCH_ASSEMBLY_EXE}
)

add_custom_target(assembly
	COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/assembly.py ${BENCH_ASSEMBLY_RESULTS}
	DEPENDS ${BENCH_ASSEMBLY_RESULTS}
)

if (likwid_FOUND)
	# this command can still fail even if likwid is installed, we need to "catch the exception" so the target `throughput` doesn't fail
	# I use the `||` trick to evaluate `cmd` and a failsafe command in case `cmd` fails
	add_custom_command(
		OUTPUT ${BENCH_MEMORY_THROUGHPUT_RESULTS}
		COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/likwid_bench.py store --num_threads 1 > ${BENCH_MEMORY_THROUGHPUT_RESULTS} || echo > ${BENCH_MEMORY_THROUGHPUT_RESULTS}
		DEPENDS ${CMAKE_SOURCE_DIR}/tests/likwid_bench.py
	)

	add_custom_target(throughput
		COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/throughput.py ${BENCH_ASSEMBLY_RESULTS} ${BENCH_MEMORY_THROUGHPUT_RESULTS}
		DEPENDS ${BENCH_ASSEMBLY_RESULTS}
		DEPENDS ${BENCH_MEMORY_THROUGHPUT_RESULTS}
	)
endif()
