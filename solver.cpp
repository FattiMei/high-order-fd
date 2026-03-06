#include <cassert>
#include <iostream>
#include "solver.h"
#include "stencil.h"


std::chrono::duration<double>
AbstractSolver::solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) {
	const auto start_time = std::chrono::high_resolution_clock::now();
	this->solve_internal(x, rhs);
	const auto end_time = std::chrono::high_resolution_clock::now();

	return end_time - start_time;
}


GeneralStencilSolver::GeneralStencilSolver(int npoints) {
	stencils = compute_laplacian_stencils<double>(npoints);

	// I'm currently open to changing ideas about
	// producing the formatted string "%d-points"
	//
	// I don't like to use std::format because it will
	// lock the user into compiling with C++20 and
	// not every system supports it
	std::stringstream ss;
	ss << npoints << "-points";

	name_internal = ss.str();
}


const std::string&
GeneralStencilSolver::name() const {
	return name_internal;
}


Eigen::SparseMatrix<double> assemble_system_matrix(const Eigen::MatrixXd& stencils, int n) {
	Eigen::SparseMatrix<double> A(n,n);
	const int nodes = stencils.cols();
	const double h = 1.0 / (n - 1.0);

	A.coeffRef(0,0) = 1.0;

	for (int i = 1; i < n-1; ++i) {
		int centerPos;

		// I don't like this implementation because
		// it's not very clear.
		//
		// I will change it soon
		if (i < nodes/2) {
			centerPos = i;
		}
		else if (n-1-i < nodes/2) {
			centerPos = nodes - (n-1-i);
		}
		else {
			centerPos = nodes/2;
		}

		for (int j = 0; j < nodes; ++j) {
			A.coeffRef(i, i-centerPos+j) = stencils(centerPos,j);
		}

		A.coeffRef(i,i) += h*h;
	}

	A.coeffRef(n-1,n-1) = 1.0;

	A.makeCompressed();
	return A;
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
