#include "problem.h"


namespace Problem {
Eigen::VectorXd compute_mesh(const int n) {
	return Eigen::VectorXd::LinSpaced(n, 0.0, 1.0);
}


Eigen::VectorXd compute_exact_solution(const Eigen::VectorXd &mesh) {
	const int n = mesh.rows();
	Eigen::VectorXd u(n);

	for (int i = 0; i < mesh.rows(); ++i) {
		u(i) = std::sin(mesh(i));
	}

	u = u / std::sin(1);

	return u;
}


Eigen::VectorXd assemble_rhs(const int n) {
	Eigen::VectorXd b = Eigen::VectorXd::Zero(n);
	b(0)   = 0.0;
	b(n-1) = 1.0;

	return b;
}
} // namespace Problem
