#ifndef __SPARSE_H__
#define __SPARSE_H__


#include <Eigen/Sparse>


class SparseSolver {
public:
	SparseSolver(int problem_size, const Eigen::MatrixXd& stencils);

	void solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs);
	void residual(Eigen::VectorXd& r, const Eigen::VectorXd& x, const Eigen::VectorXd& rhs) const;

	int get_nnz() const;

protected:
	Eigen::SparseMatrix<double> m_system_matrix;

	// the sparse solver only wants a constant matrix in column major
	// storage. This is huge because now we can use the zero copy
	// Eigen::Map<SparseMatrix<double>> for building the matrix (and possibly
	// reaching optimal performance in doing so)
	Eigen::SparseLU<Eigen::SparseMatrix<double>> m_sparse_solver;
	bool m_has_already_factorized = false;
};


Eigen::SparseMatrix<double> assemble_system_matrix(int n, const Eigen::MatrixXd&);


#endif // __SPARSE_H__
