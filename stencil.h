#ifndef __STENCIL_H__
#define __STENCIL_H__


#include <cmath>
#include <vector>
#include <concepts>
#include <eigen3/Eigen/Dense>


// these are macro definitions for a dynamic Eigen::Matrix with a custom scalar type
#define EIGEN_VECTOR(T) Eigen::Matrix<T, Eigen::Dynamic, 1>
#define EIGEN_MATRIX(T) Eigen::Matrix<T, Eigen::Dynamic, 2>


auto factorial(std::integral auto n) {
	// from the cppreference:
	//   if `num` is a natural number, `std::tgamma(num)` is the factorial of `num-1`
	return std::tgamma(n+1);
}


template <typename T>
EIGEN_VECTOR(T) compute_stencil_vandermonde(T center,
                                            int order,
                                            const std::vector<int>& nodes) {

	const auto n = nodes.size();
	EIGEN_MATRIX(T) A(n, n);
	EIGEN_VECTOR(T) b(n);

	// we don't have necessarily access to std::pow, so we build our
	// own implementation for computing:
	//   A(i,j) = nodes[i]^j
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			if (i == 0) {
				A(i,j) = static_cast<T>(1);
			}
			else {
				A(i,j) = A(i-1,j) * static_cast<T>(nodes[j]);
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
EIGEN_MATRIX(T) compute_laplacian_stencils(int n) {
	assert(n >= 3);

	EIGEN_MATRIX(T) stencils(n,n);
	const std::vector<int> nodes;

	// some stencils are recomputed because the
	// fd formula is symmetric. It's not a priority
	for (int i = 0; i < n; ++i) {
		stencils.row(i) = compute_stencil_vandermonde<T>(i, 2, nodes);
	}

	return stencils;
}


#endif // __STENCIL_H__
