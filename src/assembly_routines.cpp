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
			// here there is a dependency between the
			// for loop iterations as the insert function
			// updates the write indices
			//
			// I don't think the compiler can figure out
			// that we can remove the dependency. Maybe it
			// sees the function call and just assume there is
			// a dependency
			//
			// How can we be sure of this? Are there flags or
			// callbacks? This is yet another example of why
			// complexity is bad. We can reason about this,
			// but the compiler cannot
			//
			// Also this memory dump could be vectorized, but this
			// is yet another scheduling problem!!
			//
			// And also the memory layout of the stencils could be
			// another scheduling variable
			A.insert(i, lb+k) = operator_stencils(i-lb, k);
		}
	}

	A.insert(n-1,n-1) = 1.0;
	A.makeCompressed();

	return A;
}
