#ifndef __STENCIL_H__
#define __STENCIL_H__


#include <vector>
#include <eigen3/Eigen/Dense>


// these are macro definitions for a dynamic Eigen::Matrix with a custom scalar type
#define EIGEN_VECTOR(T) Eigen::Matrix<T, Eigen::Dynamic, 1>
#define EIGEN_MATRIX(T) Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>


template <typename T>
T factorial(T n) {
	T acc = 1;

	while (n > 1) {
		acc *= n;
		--n;
	}

	return acc;
}


template <typename T>
EIGEN_VECTOR(T) compute_stencil_vandermonde(int order,
                                            const T center,
                                            const std::vector<T>& nodes) {
	const auto n = nodes.size();
	EIGEN_MATRIX(T) A(n, n);
	EIGEN_VECTOR(T) b(n);

	// we don't have necessarily access to std::pow, so we build our
	// own implementation for computing:
	//   A(i,j) = nodes[j]^i
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			if (i == 0) {
				A(i,j) = static_cast<T>(1);
			}
			else {
				// We need to implement this operation
				A(i,j) = A(i-1,j) * (nodes[j] + (-center));
			}
		}

		b(i) = (i == order)
		     ? static_cast<T>(factorial(i))
		     : static_cast<T>(0);
	}


	// the full pivoting solver is the best choice for this case because:
	//   * we are solving small systems, so we don't care about speed
	//   * we want the highest precision possible, even if we will be
	//     using exact arithmetic for most of the time
	return A.fullPivLu().solve(b);
}


template <typename T>
EIGEN_VECTOR(T) compute_stencil_fornberg(int order,
                                         T center,
                                         const std::vector<T>& nodes) {
	// TODO
	return EIGEN_VECTOR(T)();
}


template <typename T>
EIGEN_MATRIX(T) compute_laplacian_stencils(int n) {
	assert(n >= 3);

	EIGEN_MATRIX(T) stencils(n,n);

	// modern versions of C++ could use std::ranges
	// but we will keep it simple and portable
	std::vector<T> nodes(n);
	for (int i = 0; i < n; ++i) {
		nodes[i] = static_cast<T>(i);
	}

	// some stencils are recomputed because the fd formula is symmetric.
	// It's not a priority to remove this redundancy
	for (int i = 0; i < n; ++i) {
		stencils.row(i) = compute_stencil_vandermonde<T>(2,                 // order
		                                                 static_cast<T>(i), // center
		                                                 nodes);
	}

	return stencils;
}


#endif // __STENCIL_H__
