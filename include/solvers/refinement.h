#ifndef __REFINEMENT_H__
#define __REFINEMENT_H__


#include <utility>


template <typename SolverT>
class RefinementSolver {
public:
	template <typename... Args>
	RefinementSolver(int problem_size, int maxit, double tol, Args&&... args) : m_internal_solver(std::forward<Args>(args)...),
	                                                                            m_maxit(maxit),
	                                                                            m_tol(tol),
	                                                                            m_correction(problem_size),
	                                                                            m_residual(problem_size) {}

	void solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) {
		m_internal_solver.solve(x, rhs);

		for (int it = 0; it < m_maxit; ++it) {
			m_internal_solver.residual(m_residual, x, rhs);

			if (m_residual.norm() < m_tol) {
				break;
			}

			m_internal_solver.solve(m_correction, m_residual);
			x += m_correction;
		}
	}

	void residual(Eigen::VectorXd& r, const Eigen::VectorXd& x, const Eigen::VectorXd& rhs) const {
		m_internal_solver.residual(r, x, rhs);
	}

protected:
	SolverT m_internal_solver;

	// refinement parameters
	const int m_maxit;
	const double m_tol;

	// refinement support vectors
	Eigen::VectorXd m_correction;
	Eigen::VectorXd m_residual;
};


#endif // __REFINEMENT_H__
