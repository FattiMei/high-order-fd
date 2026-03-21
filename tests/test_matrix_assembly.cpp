#include "solvers/sparse.h"
#include <iostream>


using Mat = Eigen::MatrixXd;
using SpMat = Eigen::SparseMatrix<double>;


Mat
generate_test_stencil(int stencil_size) {
	Mat res(stencil_size, stencil_size);

	for (int i = 0; i < stencil_size; ++i) {
		for (int j = 0; j < stencil_size; ++j) {
			res(i,j) = i;
		}
	}

	return res;
}



int main(int argc, char* argv[]) {
	constexpr int n = 16;

	for (int m = 3; m < 10; ++m) {
		const Mat stencil = generate_test_stencil(m);
		SpMat matrix = assemble_system_matrix(n, stencil);

		std::cout << matrix << std::endl;
	}

	return 0;
}
