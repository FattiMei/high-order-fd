#include "solvers/sparse.h"


static Eigen::SparseMatrix<double> assemble_system_matrix(int n, const Eigen::MatrixXd& laplacian_stencils) {
	const int nodes = laplacian_stencils.cols();
	const double h = 1.0 / (n - 1.0);

	// the original laplacian stencils are modified to limit the calls to
	//   SparseMatrix<double>::coeffRef(i,j)
	//
	// as it's an expensive operation
	Eigen::MatrixXd operator_stencils(laplacian_stencils);
	for (int i = 0; i < nodes; ++i) {
		operator_stencils(i,i) += h*h;
	}

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


SparseSolver::SparseSolver(int problem_size, const Eigen::MatrixXd& stencils) {
	m_system_matrix = assemble_system_matrix(problem_size, stencils);
};


void
SparseSolver::solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) {
	if (not m_has_already_factorized) {
		m_sparse_solver.compute(m_system_matrix);
		m_has_already_factorized = true;

		if (m_sparse_solver.info() != Eigen::Success) {
			std::runtime_error("LU decomposition failed");
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
