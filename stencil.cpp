#include <cassert>
#include "stencil.h"


void fill_vandermonde(EIGEN_MATRIX(StencilPrecision)& A,
                      const StencilPrecision center,
                      const std::vector<StencilPrecision>& nodes) {

	assert(A.rows() == A.cols());
	assert(A.rows() == nodes.size());

	const int n = A.rows();

	// we don't have necessarily access to std::pow, so we build our
	// own implementation for computing:
	//   A(i,j) = nodes[j]^i
	//
	// we exploit the fact that Eigen matrices are stored in col-major
	// format
	for (int j = 0; j < n; ++j) {
		const auto displacement = nodes[j] - center;
		StencilPrecision acc = 1;

		for (int i = 0; i < n; ++i) {
			A(i,j) = acc;
			acc *= displacement;
		}
	}
}


template <typename T>
T factorial(T n) {
	T acc = 1;

	while (n > 1) {
		acc *= n;
		--n;
	}

	return acc;
}


EIGEN_MATRIX(StencilPrecision) compute_laplacian_stencils(int n) {
	assert(n > 2);

	EIGEN_MATRIX(StencilPrecision) stencils(n, n);
	const int derivative_order = 2;

	// I'm sure there are better ways to
	// initialize this vector
	std::vector<StencilPrecision> nodes(n);
	for (int i = 0; i < n; ++i) {
		nodes[i] = i;
	}

	EIGEN_MATRIX(StencilPrecision) A(n, n);
	EIGEN_VECTOR(StencilPrecision) b(n);

	for (int i = 0; i < n; ++i) {
		b(i) = (i == derivative_order)
		     ? factorial(i)
		     : 0;
	}

	for (int i = 0; i < n; ++i) {
		fill_vandermonde(A, i, nodes);

		stencils.row(i) = A.fullPivLu().solve(b);
	}

	return stencils;
}
