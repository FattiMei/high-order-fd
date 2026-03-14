#ifndef __SPARSE_H__
#define __SPARSE_H__


#include <Eigen/Sparse>


class SparseSolver {
public:
	SparseSolver(int problem_size, const Eigen::MatrixXd& stencils);

	void solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs);
	void residual(Eigen::VectorXd& r, const Eigen::VectorXd& x, const Eigen::VectorXd& rhs) const;

protected:
	Eigen::SparseMatrix<double> m_system_matrix;

	Eigen::SparseLU<Eigen::SparseMatrix<double>> m_sparse_solver;
	bool m_has_already_factorized = false;
};


#endif // __SPARSE_H__
