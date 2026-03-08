#include <vector>
#include <iostream>
#include <Eigen/Core>
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
	std::vector<Discretization*> discretizations {
		new Stencil(3),
		new Stencil(5),
		new Stencil(7),
		new Stencil(9),
	};

	std::cout << "n,name,errnorm,resnorm,walltime" << std::endl;

	for (int n = 16; n < 20000; n *= 2) {
		const Eigen::VectorXd mesh = compute_mesh(n);
		const Eigen::VectorXd sol  = compute_exact_solution(mesh);
		const Eigen::VectorXd rhs  = assemble_rhs(n);
		Eigen::VectorXd res(n);
		Eigen::VectorXd x(n);

		for (const Discretization* discr : discretizations) {
			std::unique_ptr<Solver> solver = discr->generate_solver(n);

			const auto elapsed_time = solver->solve_profiled(x, rhs);
			solver->residual(res, x, rhs);

			std::cout
				<< n
				<< ','
				<< discr->name()
				<< ','
				<< (sol - x).norm()
				<< ','
				<< res.norm()
				<< ','
				<< elapsed_time
				<< std::endl;
		}
	}

	return 0;
}
