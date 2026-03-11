#ifndef __SPARSE_H__
#define __SPARSE_H__


#include "interface.h"
#include <Eigen/Sparse>


class Stencil : public SolverFactory {
public:
	Stencil(int npoints);

	virtual std::unique_ptr<Solver> generate_solver(int problem_size) const override;

private:
	Eigen::MatrixXd m_stencils;
};


class SparseSolver : public Solver {
public:
	SparseSolver(int problem_size, const Eigen::MatrixXd& stencils);

	virtual void solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) override;
	virtual void residual(Eigen::VectorXd& r, const Eigen::VectorXd& x, const Eigen::VectorXd& rhs) const override;

private:
	Eigen::SparseMatrix<double> m_system_matrix;
};


#endif // __SPARSE_H__
