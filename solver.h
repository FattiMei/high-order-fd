#ifndef __SOLVER_H__
#define __SOLVER_H__


#include <chrono>
#include <string>
#include <Eigen/Core>
#include <Eigen/Sparse>


// This abstract class is designed for:
//   * unify the possible solver implementations
//   * having an iterable container of solvers
//
// this interface let's me
//   * remove the solving logic from the main file
//   * conveniently add new solver implementations
//
// 
// IMPORTANT: since I'm planning to solve only this problem
// on a regular mesh:
//   u'' + u = 0
//   u(0) = 0
//   u(1) = 1
// the solvers will make every assumption possible
class AbstractSolver {
public:
	// measures the time elapsed for a concrete solver procedure
	//   `x` is the destination vector for the solution
	//   `rhs` is the right hand side of the general discretized problem Ax=b
	std::chrono::duration<double> solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs);

	// this must be changed, I need a constant reference
	// for the moment I'll leave it like that
	virtual const std::string name() = 0;

protected:
	virtual void solve_internal(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) = 0;
};


class GeneralStencilSolver : public AbstractSolver {
public:
	GeneralStencilSolver(int npoints);

	virtual const std::string name() override {
		// this is obv a mock implementation
		return "";
	}

protected:
	virtual void solve_internal(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) override;

private:
	Eigen::SparseLU<Eigen::SparseMatrix<double>> sparse_solver;
	Eigen::MatrixXd stencils;
};


#endif // __SOLVER_H__
