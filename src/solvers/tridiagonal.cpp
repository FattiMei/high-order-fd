#include "solvers/tridiagonal.h"
#include <lapacke.h>


Tridiagonal::Tridiagonal() {
	m_name = "tridiag";
}


std::unique_ptr<Solver>
Tridiagonal::generate_solver(int problem_size) const {
	std::unique_ptr<Solver> p = std::make_unique<TridiagonalSolver>(problem_size);

	return p;
}



TridiagonalSolver::TridiagonalSolver(int problem_size) : m_dl(problem_size-1),
                                                         m_d (problem_size),
                                                         m_du(problem_size-1) {
	// there are some shenanigans because the first and
	// last rows of the matrix include the boundary conditions
	// and are a little special
	m_dl.fill(1.0);
	m_dl[problem_size-2] = 0.0;

	m_du.fill(1.0);
	m_du[0] = 0.0;

	const double h = 1.0 / (problem_size - 1.0);
	m_d.fill(-2.0 + h*h);
	m_d[0] = 1.0;
	m_d[problem_size-1] = 1.0;
}


void
TridiagonalSolver::solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) {
	x = rhs;

	LAPACKE_dgtsv(LAPACK_ROW_MAJOR,
	              x.size(),
	              1,
	              m_dl.data(),
	              m_d.data(),
	              m_du.data(),
	              x.data(),
	              1);
}


void
TridiagonalSolver::residual(Eigen::VectorXd& r, const Eigen::VectorXd& x, const Eigen::VectorXd& rhs) const {
	const int n = x.size();
	const double h = 1.0 / (n - 1.0);

	r[0] = rhs[0] - x[0];
	r[n-1] = rhs[n-1] - x[n-1];

	for (int i = 1; i < n-1; ++i) {
		r[i] = rhs[i] - (x[i-1] - 2.0 * h*h * x[i] - x[i+1]);
	}
}
