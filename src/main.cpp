#include <chrono>
#include <iostream>
#include "problem.h"
#include "solvers/sparse.h"
#include "solvers/tridiagonal.h"


#define TIC() std::chrono::high_resolution_clock::now()




int main(int argc, char* argv[]) {
	int MAX_PROBLEM_SIZE = 20'000;
	if (argc > 1) {
		MAX_PROBLEM_SIZE = std::stoi(argv[1]);
	}

	std::cout << "n,name,errnorm,resnorm,assemble_time_s,solve_time_s,total_time_s" << std::endl;

	for (int n = 16; n < MAX_PROBLEM_SIZE; n *= 2) {
		const Eigen::VectorXd mesh = Problem::compute_mesh(n);
		const Eigen::VectorXd sol  = Problem::compute_exact_solution(mesh);
		const Eigen::VectorXd rhs  = Problem::assemble_rhs(n);
		Eigen::VectorXd res(n);
		Eigen::VectorXd x(n);

		#define PROFILE_SOLVER(solver_recipe) do {                                                           \
			const auto assemble_start_time = TIC();                                                      \
			auto solver = solver_recipe;                                                                 \
			const auto assemble_end_time = TIC();                                                        \
			const std::chrono::duration<double> assemble_time = assemble_end_time - assemble_start_time; \
		                                                                                                     \
			const auto solve_start_time = TIC();                                                         \
			solver.solve(x, rhs);                                                                        \
			const auto solve_end_time = TIC();                                                           \
			const std::chrono::duration<double> solve_time = solve_end_time - solve_start_time;          \
		                                                                                                     \
			const std::chrono::duration<double> total_time = assemble_time + solve_time;                 \
		                                                                                                     \
			solver.residual(res, x, rhs);                                                                \
		                                                                                                     \
			std::cout                                                                                    \
				<< n                                                                                 \
				<< ','                                                                               \
				<< "solver_recipe"                                                                   \
				<< ','                                                                               \
				<< (sol - x).norm()                                                                  \
				<< ','                                                                               \
				<< res.norm()                                                                        \
				<< ','                                                                               \
				<< assemble_time.count()                                                             \
				<< ','                                                                               \
				<< solve_time.count()                                                                \
				<< ','                                                                               \
				<< total_time.count()                                                                \
				<< std::endl;                                                                        \
		} while (0)                                                                                          \

		PROFILE_SOLVER(TridiagonalSolver(n));
	}

	return 0;
}
