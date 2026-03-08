#include <cassert>
#include <iostream>
#include "solver.h"
#include "stencil.h"


std::chrono::duration<double>
Solver::solve_profiled(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) {
	const auto start_time = std::chrono::high_resolution_clock::now();
	this->solve(x, rhs);
	const auto end_time = std::chrono::high_resolution_clock::now();

	return end_time - start_time;
}


static Eigen::SparseMatrix<double> assemble_system_matrix(int n, const Eigen::MatrixXd& stencils) {
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


SparseSolver::SparseSolver(int problem_size, const Eigen::MatrixXd& stencils) {
	m_system_matrix = assemble_system_matrix(problem_size, stencils);
};


void
SparseSolver::solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) {
	Eigen::SparseLU<Eigen::SparseMatrix<double>> sparse_solver;

	sparse_solver.compute(m_system_matrix);
	if (sparse_solver.info() != Eigen::Success) {
		std::cerr << "Decomposition failed" << std::endl;

		// this should crash the program, I'm still deciding
		// between throwing an exception or calling exit()
		assert(false);
	}

	x = sparse_solver.solve(rhs);
}


void
SparseSolver::residual(Eigen::VectorXd& r, const Eigen::VectorXd& x, const Eigen::VectorXd& rhs) const {
	r = rhs - m_system_matrix * x;
}


Stencil::Stencil(int npoints) {
	m_stencils = compute_laplacian_stencils(npoints);

	std::stringstream ss;
	ss << npoints << "-points";

	m_name = ss.str();
}


std::unique_ptr<Solver>
Stencil::generate_solver(int problem_size) const {
	std::unique_ptr<Solver> p = std::make_unique<SparseSolver>(problem_size, m_stencils);

	return p;
}


const std::string&
Stencil::name() const {
	return m_name;
}
