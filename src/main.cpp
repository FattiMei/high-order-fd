#include <chrono>
#include <iostream>
#include "problem.h"
#include "stencil.h"
#include "solvers/sparse.h"
#include "solvers/refinement.h"
#include "solvers/tridiagonal.h"


#define TIC() std::chrono::high_resolution_clock::now()


int main(int argc, char* argv[]) {
	int MAX_PROBLEM_SIZE = 20'000;
	if (argc > 1) {
		MAX_PROBLEM_SIZE = std::stoi(argv[1]);
	}

	const Eigen::MatrixXd laplacian_3_point = compute_laplacian_stencils(3);
	const Eigen::MatrixXd laplacian_5_point = compute_laplacian_stencils(5);
	const Eigen::MatrixXd laplacian_7_point = compute_laplacian_stencils(7);
	const Eigen::MatrixXd laplacian_9_point = compute_laplacian_stencils(9);

	std::cout << "n,name,errnorm,resnorm,nnz,assembly_time_s,solve_time_s,total_time_s" << std::endl;

	for (int n = 16; n < MAX_PROBLEM_SIZE; n *= 2) {
		const Eigen::VectorXd mesh = Problem::compute_mesh(n);
		const Eigen::VectorXd sol  = Problem::compute_exact_solution(mesh);
		const Eigen::VectorXd rhs  = Problem::assemble_rhs(n);
		Eigen::VectorXd res(n);
		Eigen::VectorXd x(n);

		#define RUN_EXPERIMENT(solver_recipe, name) do {                                                     \
			const auto assembly_start_time = TIC();                                                      \
			auto solver = solver_recipe;                                                                 \
			const auto assembly_end_time = TIC();                                                        \
			const std::chrono::duration<double> assembly_time = assembly_end_time - assembly_start_time; \
		                                                                                                     \
			const auto solve_start_time = TIC();                                                         \
			solver.solve(x, rhs);                                                                        \
			const auto solve_end_time = TIC();                                                           \
			const std::chrono::duration<double> solve_time = solve_end_time - solve_start_time;          \
		                                                                                                     \
			const std::chrono::duration<double> total_time = assembly_time + solve_time;                 \
		                                                                                                     \
			solver.residual(res, x, rhs);                                                                \
		                                                                                                     \
			std::cout                                                                                    \
				<< n                                                                                 \
				<< ','                                                                               \
				<< name                                                                              \
				<< ','                                                                               \
				<< (sol - x).norm()                                                                  \
				<< ','                                                                               \
				<< res.norm()                                                                        \
				<< ','                                                                               \
				<< solver.get_nnz()                                                                  \
				<< ','                                                                               \
				<< assembly_time.count()                                                             \
				<< ','                                                                               \
				<< solve_time.count()                                                                \
				<< ','                                                                               \
				<< total_time.count()                                                                \
				<< std::endl;                                                                        \
		} while (0)                                                                                          \

		// this macro behaves like a solver factory: it allows me
		// to control where the solver needs to be constructed by
		// providing the recipe
		//
		// the macro code only needs compile time polymorphism
		RUN_EXPERIMENT(SparseSolver(n, laplacian_3_point), "3-point");
		RUN_EXPERIMENT(SparseSolver(n, laplacian_5_point), "5-point");
		RUN_EXPERIMENT(SparseSolver(n, laplacian_7_point), "7-point");
		RUN_EXPERIMENT(SparseSolver(n, laplacian_9_point), "9-point");

		RUN_EXPERIMENT(TridiagonalSolver(n), "tridiag");
	}

	return 0;
}
