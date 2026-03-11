#ifndef __TRIDIAGONAL_H__
#define __TRIDIAGONAL_H__


#include "interface.h"


class Tridiagonal : public SolverFactory {
public:
	Tridiagonal();
	virtual std::unique_ptr<Solver> generate_solver(int problem_size) const override;
};


class TridiagonalSolver : public Solver {
public:
	TridiagonalSolver(int problem_size);

	virtual void solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) override;
	virtual void residual(Eigen::VectorXd& r, const Eigen::VectorXd& x, const Eigen::VectorXd& rhs) const override;

private:
};


#endif // __TRIDIAGONAL_H__
