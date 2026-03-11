#include <chrono>
#include <vector>
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

	std::vector<std::unique_ptr<SolverFactory>> solver_factories;
	solver_factories.push_back(std::make_unique<Stencil>(3));
	solver_factories.push_back(std::make_unique<Stencil>(5));
	solver_factories.push_back(std::make_unique<Stencil>(7));
	solver_factories.push_back(std::make_unique<Stencil>(9));

	std::cout << "n,name,errnorm,resnorm,assemble_time,solve_time" << std::endl;

	for (int n = 16; n < MAX_PROBLEM_SIZE; n *= 2) {
		const Eigen::VectorXd mesh = Problem::compute_mesh(n);
		const Eigen::VectorXd sol  = Problem::compute_exact_solution(mesh);
		const Eigen::VectorXd rhs  = Problem::assemble_rhs(n);
		Eigen::VectorXd res(n);
		Eigen::VectorXd x(n);

		for (const auto& factory : solver_factories) {
			const auto assemble_start_time = TIC();
			std::unique_ptr<Solver> solver = factory->generate_solver(n);
			const auto assemble_end_time = TIC();
			const std::chrono::duration<double> assemble_time = assemble_end_time - assemble_start_time;

			const auto solve_start_time = TIC();
			solver->solve(x, rhs);
			const auto solve_end_time = TIC();
			const std::chrono::duration<double> solve_time = solve_end_time - solve_start_time;

			solver->residual(res, x, rhs);

			std::cout
				<< n
				<< ','
				<< factory->name()
				<< ','
				<< (sol - x).norm()
				<< ','
				<< res.norm()
				<< ','
				<< assemble_time
				<< ','
				<< solve_time
				<< std::endl;
		}
	}

	return 0;
}
