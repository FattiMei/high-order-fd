#include "solvers/sparse.h"
#include <cassert>


Eigen::SparseMatrix<double>
assemble_system_matrix_reference(int n, const Eigen::MatrixXd& operator_stencils) {
	const int nodes = operator_stencils.cols();

	Eigen::SparseMatrix<double> A(n,n);
	A.reserve(Eigen::VectorXi::Constant(n, nodes));

	A.insert(0,0) = 1.0;

	// we are trying to assemble the matrix in a column-major format
	// this is a gateway to directly produce the compressed data structures
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
			A.insert(i, i-centerPos+j) = operator_stencils(centerPos,j);
		}
	}

	A.insert(n-1,n-1) = 1.0;
	A.makeCompressed();

	return A;
}


Eigen::SparseMatrix<double>
assemble_system_matrix_optimized(int n, const Eigen::MatrixXd& operator_stencils) {
	const int m = operator_stencils.rows();

	Eigen::SparseMatrix<double> A(n,n);
	A.reserve(Eigen::VectorXi::Constant(n, m));

	A.insert(0,0) = 1.0;

	// we are trying to assemble the matrix in a column-major format
	// this is a gateway to directly produce the compressed data structures
	for (int i = 1; i < n-1; ++i) {
		const int lb = std::min(
			std::max(0, i - m/2), // handles the left boundary
			n-m                   // handles the right boundary
		);

		for (int k = 0; k < m; ++k) {
			A.insert(i, lb+k) = operator_stencils(i-lb, k);
		}
	}

	A.insert(n-1,n-1) = 1.0;
	A.makeCompressed();

	return A;
}


SparseSolver::SparseSolver(int problem_size, const Eigen::MatrixXd& laplacian_stencils) {
	assert(laplacian_stencils.rows() == laplacian_stencils.cols());
	assert(problem_size > laplacian_stencils.cols());

	const double h = 1.0 / (problem_size-1);

	Eigen::MatrixXd operator_stencils(laplacian_stencils);
	for (int i = 0; i < laplacian_stencils.rows(); ++i) {
		operator_stencils(i,i) += h*h;
	}

	m_system_matrix = assemble_system_matrix_optimized(problem_size, operator_stencils);
};


void
SparseSolver::solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) {
	if (not m_has_already_factorized) {
		m_sparse_solver.compute(m_system_matrix);
		m_has_already_factorized = true;

		if (m_sparse_solver.info() != Eigen::Success) {
			assert(false);
			throw std::runtime_error("LU decomposition failed");
		}
	}

	x = m_sparse_solver.solve(rhs);
}


void
SparseSolver::residual(Eigen::VectorXd& r, const Eigen::VectorXd& x, const Eigen::VectorXd& rhs) const {
	r = rhs - m_system_matrix * x;
}


int
SparseSolver::get_nnz() const {
	return m_system_matrix.nonZeros();
}
