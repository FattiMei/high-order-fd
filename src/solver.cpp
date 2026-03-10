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


// at the moment we are exploring the possibility of building
// the compressed sparse matrix directly from the internal data structures.
//
// if we understand a little bit better the way stencils are copied
// into the sparse matrix we could build in place the right data structures.
// doing so will remove the bulk of the memory transactions
static Eigen::SparseMatrix<double> assemble_system_matrix(int n, Eigen::MatrixXd stencils) {
	const int nodes = stencils.cols();
	const double h = 1.0 / (n - 1.0);

	// I'm adding the contribution of the u term in the
	// operator u'' + u
	//
	// I modify the original stencil which is passed by value
	// in order to remove every call to `A.coeffRef(i,j)` as
	// it's a O(nnz) operation
	for (int i = 0; i < nodes; ++i) {
		stencils(i,i) += h*h;
	}

	Eigen::SparseMatrix<double> A(n,n);
	A.reserve(Eigen::VectorXi::Constant(n, nodes));

	A.insert(0,0) = 1.0;

	for (int i = 1; i < n-1; ++i) {
		int centerPos;

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
			A.insert(i, i-centerPos+j) = stencils(centerPos,j);
		}
	}

	A.insert(n-1,n-1) = 1.0;
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
