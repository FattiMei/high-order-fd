#include "solvers/sparse.h"
#include <cassert>


static Eigen::SparseMatrix<double> assemble_system_matrix(int n, const Eigen::MatrixXd& laplacian_stencils) {
	assert(laplacian_stencils.rows() == laplacian_stencils.cols());
	assert(n > laplacian_stencils.cols());

	const int m = laplacian_stencils.cols();

	const double h = 1.0 / (n - 1.0);

	// the original laplacian stencils are modified to limit the calls to
	//   SparseMatrix<double>::coeffRef(i,j)
	//
	// as it's an expensive operation
	Eigen::MatrixXd operator_stencils(laplacian_stencils);
	for (int i = 0; i < m; ++i) {
		operator_stencils(i,i) += h*h;
	}

	Eigen::SparseMatrix<double> A(n,n);
	A.reserve(Eigen::VectorXi::Constant(n, m));

	A.insert(0,0) = 1.0;

	// we are trying to assemble the matrix in a column-major format
	// this is a gateway to directly produce the compressed data structures
	for (int i = 1; i < n-1; ++i) {
		int lb, ub;

		const int left_neighbours = i;
		const int right_neighbours = n-1-i;

		if (left_neighbours < m/2) {
			lb = 0;
			ub = m;
		} else if (right_neighbours < m/2) {
			ub = n;
			lb = n-m;
		} else {
			lb = i - m/2;
			ub = lb + m;
		}

		for (int j = lb; j < ub; ++j) {
			assert(j >= 0);
			assert(j < n);
			assert(i-lb >= 0);
			assert(i-lb < m);

			A.insert(i,j) = operator_stencils(i-lb, j-lb);
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
