#ifndef __PROBLEM_H__
#define __PROBLEM_H__


#include <Eigen/Core>


namespace Problem {
	Eigen::VectorXd compute_mesh(int n);
	Eigen::VectorXd compute_exact_solution(const Eigen::VectorXd& mesh);
	Eigen::VectorXd assemble_rhs(int n);
}


#endif // __PROBLEM_H__
