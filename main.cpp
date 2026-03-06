#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include "solver.h"


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


int main(int argc, char* argv[]) {
	std::vector<AbstractSolver*> solvers{
		new GeneralStencilSolver(3),
		new GeneralStencilSolver(5),
		new GeneralStencilSolver(7),
		new GeneralStencilSolver(9),
	};

	std::cout << "n,order,errnorm,residual,walltime[ns]" << std::endl;

	for (int n = 16; n < 20000; n *= 2) {
		Eigen::VectorXd mesh = compute_mesh(n);
		Eigen::VectorXd sol  = compute_exact_solution(mesh);
		Eigen::VectorXd b    = assemble_rhs(n);
		Eigen::VectorXd x(n);

		for (AbstractSolver* solver : solvers) {
			const auto elapsed_time = solver->solve(x, b);

			std::cout
				<< n
				<< ','
				<< solver->name()
				<< ','
				<< (sol - x).norm()
				<< ','
				<< 0 // and this should be the residual
				<< ','
				<< elapsed_time
				<< std::endl;
		}
	}

	return 0;
}
