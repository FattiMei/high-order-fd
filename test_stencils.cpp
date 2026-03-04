#include <vector>
#include <cassert>
#include <iostream>
#include <boost/rational.hpp>
#include "stencil.h"


// this file tests some finite difference stencils
// against the ones reported in the Fornberg paper at
// https://www.colorado.edu/amath/sites/default/files/attached-files/mathcomp_88_fd_formulas.pdf
//
// I wonder if there is a good way of performing such tests.
// For example one could specify in a declarative way:
//   * the order of derivative
//   * the nodes
//   * the coefficients
// and the system would be able to automatically generate the tests.
//
// Right now there is some degree of boilerplate and repetition.
// I'm quite confident in the correctness of my implementation, but
// it's definitely worth a try
int main() {
	using R = boost::rational<int>;

	// first derivative, centered at 0
	{
		const R center = 0;

		{
			const std::vector<R> nodes{-1, 0, 1};
			EIGEN_VECTOR(R) expected(nodes.size());
			expected << R(-1,2), R(0), R(1,2);

			const auto computed = compute_stencil_vandermonde(1, center, nodes);

			assert(expected == computed);
		}

		{
			const std::vector<R> nodes{-2, -1, 0, 1, 2};
			EIGEN_VECTOR(R) expected(nodes.size());
			expected << R(1,12), R(-2,3), R(0), R(2,3), R(-1,12);

			const auto computed = compute_stencil_vandermonde(1, center, nodes);

			assert(expected == computed);
		}
	}

	// second derivative, centered at 0
	{
		const R center = 0;

		{
			const std::vector<R> nodes{-1, 0, 1};
			EIGEN_VECTOR(R) expected(nodes.size());
			expected << R(1), R(-2), R(1);

			const auto computed = compute_stencil_vandermonde(2, center, nodes);

			assert(expected == computed);
		}

		{
			const std::vector<R> nodes{-2, -1, 0, 1, 2};
			EIGEN_VECTOR(R) expected(nodes.size());
			expected << R(-1,12), R(4,3), R(-5,2), R(4,3), R(-1,12);

			const auto computed = compute_stencil_vandermonde(2, center, nodes);

			assert(expected == computed);
		}

		{
			const std::vector<R> nodes{-3, -2, -1, 0, 1, 2, 3};
			EIGEN_VECTOR(R) expected(nodes.size());
			expected << R(1,90), R(-3,20), R(3,2), R(-49,18), R(3,2), R(-3,20), R(1,90);

			const auto computed = compute_stencil_vandermonde(2, center, nodes);

			assert(expected == computed);
		}
	}

	return 0;
}
