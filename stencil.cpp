#include <cassert>
#include "stencil.h"


#ifdef USE_EXACT_STENCILS
#include "rational.h"
using StencilPrecision = rational<int64_t>;
#else
using StencilPrecision = double;
#endif


#define EIGEN_VECTOR(T) Eigen::Matrix<T, Eigen::Dynamic, 1>
#define EIGEN_MATRIX(T) Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>


void fill_vandermonde(EIGEN_MATRIX(StencilPrecision)& A,
                      const StencilPrecision center,
                      const std::vector<StencilPrecision>& nodes) {

	assert(A.rows() == A.cols());
	assert(A.rows() == nodes.size());

	const int n = A.rows();

	// we might not have access to std::pow, so we build our
	// own implementation for computing:
	//   A(i,j) = nodes[j]^i
	//
	// we exploit the fact that Eigen matrices are stored in col-major
	// format
	for (int j = 0; j < n; ++j) {
		const auto displacement = nodes[j] + (-center);
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


// the laplacian stencils are used to assemble a sparse
// matrix which will always be Eigen::SparseMatrix<double>
Eigen::MatrixXd compute_laplacian_stencils(int n) {
	assert(n > 2);

	const int derivative_order = 2;
	EIGEN_MATRIX(StencilPrecision) stencils(n, n);
	EIGEN_MATRIX(StencilPrecision) A(n, n);
	EIGEN_VECTOR(StencilPrecision) b(n);

	std::vector<StencilPrecision> nodes(n);
	for (int i = 0; i < n; ++i) {
		nodes[i] = i;

		b(i) = (i == derivative_order)
		     ? factorial(i)
		     : 0;
	}

	// this implementation exposes details of the stencil calculation,
	// but it's a good thing because it saves me some memory allocations
	// and redundant initializations
	for (int i = 0; i < n; ++i) {
		fill_vandermonde(A, i, nodes);

		stencils.row(i) = A.fullPivLu().solve(b);
	}

	// I can't make the MatrixBase::cast<NewType> work,
	// so I write this simple implementation
	//
	// TODO: understand this better
	Eigen::MatrixXd result(n,n);
	for (int j = 0; j < n; ++j) {
		for (int i = 0; i < n; ++i) {
			result(i,j) = static_cast<double>(stencils(i,j));
		}
	}

	return result;
}
