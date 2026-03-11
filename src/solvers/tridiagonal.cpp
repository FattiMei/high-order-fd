#include "solvers/tridiagonal.h"


Tridiagonal::Tridiagonal() {
	m_name = "tridiag";
}


std::unique_ptr<Solver>
Tridiagonal::generate_solver(int problem_size) const {
	std::unique_ptr<Solver> p = std::make_unique<TridiagonalSolver>(problem_size);

	return p;
}



TridiagonalSolver::TridiagonalSolver(int problem_size) {
	// TODO: some memory allocations are required
}


void
TridiagonalSolver::solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) {

}


void
TridiagonalSolver::residual(Eigen::VectorXd& r, const Eigen::VectorXd& x, const Eigen::VectorXd& rhs) const {

}
