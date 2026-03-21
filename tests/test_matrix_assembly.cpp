#include "solvers/sparse.h"


int main(int argc, char* argv[]) {
	using Mat = Eigen::MatrixXd;
	using SpMat = Eigen::SparseMatrix<double>;

	int MAX_PROBLEM_SIZE = 20'000;
	if (argc > 1) {
		MAX_PROBLEM_SIZE = std::stoi(argv[1]);
	}

	for (int n = 16; n < MAX_PROBLEM_SIZE; n *= 2) {
		for (int m = 3; m < 10; ++m) {
			const Mat stencil = Eigen::MatrixXd::Random(m,m);

			const SpMat reference = assemble_system_matrix_reference(n, stencil);
			const SpMat alternative = assemble_system_matrix_optimized(n, stencil);

			const SpMat diff = reference - alternative;
			if (diff.norm() > 0.0) {
				return 1;
			}
		}
	}

	return 0;
}
