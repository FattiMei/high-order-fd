#ifndef __TRIDIAGONAL_H__
#define __TRIDIAGONAL_H__


#include <Eigen/Core>


class TridiagonalSolver {
public:
	TridiagonalSolver(int problem_size);

	void solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs);
	void residual(Eigen::VectorXd& r, const Eigen::VectorXd& x, const Eigen::VectorXd& rhs) const;
	int get_nnz() const;

protected:
	Eigen::VectorXd m_dl;
	Eigen::VectorXd m_d ;
	Eigen::VectorXd m_du;
};


#endif // __TRIDIAGONAL_H__
