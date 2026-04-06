#include "assembly.h"
#include "solvers/sparse.h"
#include <cassert>


SparseSolver::SparseSolver(int problem_size, const Eigen::MatrixXd& laplacian_stencils) {
	assert(laplacian_stencils.rows() == laplacian_stencils.cols());
	assert(problem_size > laplacian_stencils.cols());

	const double h = 1.0 / (problem_size-1);

	Eigen::MatrixXd operator_stencils(laplacian_stencils);
	for (int i = 0; i < laplacian_stencils.rows(); ++i) {
		operator_stencils(i,i) += h*h;
	}

	m_system_matrix = assemble_system_matrix<>(problem_size, operator_stencils);
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
