#ifndef __INTERFACE_H__
#define __INTERFACE_H__


#include <memory>
#include <string>
#include <Eigen/Core>


class Solver {
public:
	virtual void solve(Eigen::VectorXd& x, const Eigen::VectorXd& rhs) = 0;
	virtual void residual(Eigen::VectorXd& r, const Eigen::VectorXd& x, const Eigen::VectorXd& rhs) const = 0;
};


class SolverFactory {
public:
	virtual std::unique_ptr<Solver> generate_solver(int problem_size) const = 0;
	const std::string& name() { return m_name; }

protected:
	std::string m_name;
};


#endif // __INTERFACE_H__
