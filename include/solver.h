#ifndef __SOLVER_H__
#define __SOLVER_H__


#include <chrono>
#include <memory>
#include <string>
#include <Eigen/Core>
#include <Eigen/Sparse>


class Solver {
public:
	virtual void solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) = 0;
	virtual void residual(Eigen::VectorXd& r, const Eigen::VectorXd& x, const Eigen::VectorXd& rhs) const = 0;

	std::chrono::duration<double> solve_profiled(Eigen::VectorXd& x, const Eigen::VectorXd& rhs);
};


class SparseSolver : public Solver {
public:
	SparseSolver(int problem_size, const Eigen::MatrixXd& stencils);

	virtual void solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) override;
	virtual void residual(Eigen::VectorXd& r, const Eigen::VectorXd& x, const Eigen::VectorXd& rhs) const override;

private:
	Eigen::SparseMatrix<double> m_system_matrix;
};


// This solver is proposed only as an example of a specialized, optimized solver and
// its performance has to be compared with high order finite differences.
//
// I'm not particularly interested in writing a tridiagonal solver, so I'll use
// an external dependency
class TridiagonalSolver : public Solver {
public:
	TridiagonalSolver(int problem_size);

	virtual void solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) override;
	virtual void residual(Eigen::VectorXd& r, const Eigen::VectorXd& x, const Eigen::VectorXd& rhs) const override;

private:
};


// A solver is constructed for every problem size, so it's impractical to compute
// problem independent quantities in the solver constructor.
//
// The Discretization is a solver factory that remove the redundancies and allow
// the user to choose which solvers to use without explicitly constructing one
class Discretization {
public:
	virtual std::unique_ptr<Solver> generate_solver(int problem_size) const = 0;
	virtual const std::string& name() const = 0;
};


class Stencil : public Discretization {
public:
	Stencil(int npoints);

	virtual std::unique_ptr<Solver> generate_solver(int problem_size) const override;
	virtual const std::string& name() const override;

private:
	Eigen::MatrixXd m_stencils;
	std::string m_name;
};


class Tridiagonal : public Discretization {
public:
	Tridiagonal() {};

	virtual std::unique_ptr<Solver> generate_solver(int problem_size) const override;
};


#endif // __SOLVER_H__
