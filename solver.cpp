#include <cassert>
#include <iostream>
#include "solver.h"


std::chrono::duration<double>
AbstractSolver::solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) {
	const auto start_time = std::chrono::high_resolution_clock::now();
	this->solve_internal(x, rhs);
	const auto end_time = std::chrono::high_resolution_clock::now();

	return end_time - start_time;
}


GeneralStencilSolver::GeneralStencilSolver(int npoints) {
}


Eigen::SparseMatrix<double> assemble_system_matrix(const Eigen::MatrixXd& stencils, int n) {

}


void
GeneralStencilSolver::solve_internal(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) {
	const auto A = assemble_system_matrix(stencils, x.size());

	sparse_solver.compute(A);
	if (sparse_solver.info() != Eigen::Success) {
		std::cerr << "Decomposition failed" << std::endl;

		// this should crash the program, I'm still deciding
		// between throwing an exception or calling exit()
		assert(false);
	}

	x = sparse_solver.solve(rhs);
}
