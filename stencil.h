#ifndef __STENCIL_H__
#define __STENCIL_H__


#include <cmath>
#include <cassert>
#include <eigen3/Eigen/Dense>


template <typename Integral>
Integral factorial(const Integral n) {
	Integral acc = 1;

	for (Integral k = n; k > 0; --k) {
		acc *= k;
	}

	return k;
}


template <typename T = double>
Eigen::VectorXd compute_stencil_vandermonde(const T center, const int order, const std::vector<int> &nodes) {
	const auto npoints = nodes.length();

	Eigen::MatrixXd A(npoints, npoints);
	Eigen::VectorXd b(npoints);

	for (int i = 0; i < npoints; ++i) {
		for (int j = 0; j < npoints; ++j) {
			// std::pow is not compatible with all the possible `T`
			// this needs to be changed
			A(i,j) = std::pow(nodes[i] - center, j);
		}

		b(i) = (i == order) ? factorial(i) : 0;
	}
}


// `T` is the working precision
// ideally, this function should be templated on the stencil calculator, which may have some internal state
template <typename T>
Eigen::MatrixXd compute_laplacian_stencils(const int npoints) {
	assert(npoints >= 3);
	assert(npoints % 2 == 1);
}


#endif // __STENCIL_H__
