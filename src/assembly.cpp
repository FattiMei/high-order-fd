#include "assembly.h"


Eigen::SparseMatrix<double>
assemble_system_matrix_original(int n, const Eigen::MatrixXd& operator_stencils) {
	const int m = operator_stencils.rows();

	Eigen::SparseMatrix<double> A(n,n);
	A.reserve(Eigen::VectorXi::Constant(n, m));

	A.insert(0,0) = 1.0;

	// we are trying to assemble the matrix in a column-major format
	// this is a gateway to directly produce the compressed data structures
	for (int i = 1; i < n-1; ++i) {
		const int lb = std::min(
			std::max(0, i - m/2), // handles the left boundary
			n-m                   // handles the right boundary
		);

		for (int k = 0; k < m; ++k) {
			A.insert(i, lb+k) = operator_stencils(i-lb, k);
		}
	}

	A.insert(n-1,n-1) = 1.0;
	A.makeCompressed();

	return A;
}
