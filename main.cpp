#include <chrono>
#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include "stencil.h"
#include "rational.h"


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


template <typename T>
Eigen::SparseMatrix<double> assemble_matrix(const EIGEN_MATRIX(T) &stencil, const int n) {
	Eigen::SparseMatrix<double> A(n,n);
	const int nodes = stencil.cols();
	const double h = 1.0 / (n - 1.0);

	A.coeffRef(0,0) = 1.0;

	for (int i = 1; i < n-1; ++i) {
		int centerPos;

		if (i < nodes/2) {
			centerPos = i;
		}
		else if (n-1-i < nodes/2) {
			centerPos = nodes - (n-1-i);
		}
		else {
			centerPos = nodes/2;
		}

		for (int j = 0; j < nodes; ++j) {
			A.coeffRef(i, i-centerPos+j) = stencil(centerPos,j);
		}

		A.coeffRef(i,i) += h*h;
	}


	A.coeffRef(n-1,n-1) = 1.0;

	A.makeCompressed();
	return A;
}


int main(int argc, char* argv[]) {
	using rational = rational<long>;
	std::vector<std::pair<int, EIGEN_MATRIX(rational)>> stencils{
		{3, compute_laplacian_stencils<rational>(3)},
		{5, compute_laplacian_stencils<rational>(5)},
		{7, compute_laplacian_stencils<rational>(7)},
		{9, compute_laplacian_stencils<rational>(9)}
	};

	std::cout << "n,order,errnorm,residual,walltime[ns]" << std::endl;

	for (int n = 16; n < 20000; n *= 2) {
		Eigen::VectorXd mesh = compute_mesh(n);
		Eigen::VectorXd sol  = compute_exact_solution(mesh);
		Eigen::VectorXd b    = assemble_rhs(n);

		for (auto &[points, stencil] : stencils) {
			const auto start = std::chrono::high_resolution_clock::now();

			Eigen::SparseMatrix<double> A = assemble_matrix(stencil, n);
			Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
			solver.compute(A);

			if (solver.info() != Eigen::Success) {
				std::cerr << "Decomposition failed" << std::endl;
				return -1;
			}

			const Eigen::VectorXd x = solver.solve(b);

			const auto delta = std::chrono::high_resolution_clock::now() - start;

			std::cout
				<< n
				<< ','
				<< points
				<< ','
				<< (sol - x).norm()
				<< ','
				<< (b - A*x).norm()
				<< ','
				<< std::chrono::duration_cast<std::chrono::nanoseconds>(delta).count()
				<< std::endl;
		}
	}

	return 0;
}
