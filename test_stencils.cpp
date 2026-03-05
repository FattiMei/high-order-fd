#include <vector>
#include <cassert>
#include <iostream>
#include <boost/rational.hpp>
#include "stencil.h"


// this file tests some finite difference stencils
// against the ones reported in the Fornberg paper at
//   https://www.colorado.edu/amath/sites/default/files/attached-files/mathcomp_88_fd_formulas.pdf
//
// every stencil here assumes the approximations for x = 0


using Rational = boost::rational<long long>;
#define R(num,den) Rational(num,den)


struct Stencil {
	const int derivative_order;
	const std::vector<Rational> nodes;
	const std::vector<Rational> weights;
};


const std::vector<Stencil> test_cases {
	{
		1,                   // order of derivative
		{-1, 0, 1},          // nodes
		{R(-1,2), 0, R(1,2)} // weights
	},
	{
		1,
		{-2, -1, 0, 1, 2},
		{R(1,12), R(-2,3), 0, R(2,3), R(-1,12)}
	},
	{
		1,
		{-3, -2, -1, 0, 1, 2, 3},
		{R(-1,60), R(3,20), R(-3,4), 0, R(3,4), R(-3,20), R(1,60)}
	},
	{
		1,
		{-4, -3, -2, -1, 0, 1, 2, 3, 4},
		{R(1,280), R(-4,105), R(1,5), R(-4,5), 0, R(4,5), R(-1,5), R(4,105), R(-1,280)}
	},
	{
		2,
		{-1, 0, 1},
		{1, -2, 1}
	},
	{
		2,
		{-2, 1, 0, 1, 2},
		{R(-1,12), R(4,3), R(-5,2), R(4,3), R(-1,12)}
	},
	{
		0,
		{R(-7,2), R(-5,2), R(-3,2), R(-1,2), R(1,2), R(3,2), R(5,2), R(7,2)},
		{R(-5,2048), R(49,2048), R(-245,2048), R(1225,2048), R(1225,2048), R(-245,2048), R(49,2048), R(-5,2048)}
	},
};


template <typename T>
EIGEN_VECTOR(T) std_vector_to_eigen(const std::vector<T>& v) {
	EIGEN_VECTOR(T) result(v.size());

	for (int i = 0; i < v.size(); ++i) {
		result(i) = v[i];
	}

	return result;
}


int main() {
	bool ok = true;

	for (const auto& stencil : test_cases) {
		const Rational center = 0;
		const auto& derivative_order = stencil.derivative_order;
		const auto& nodes = stencil.nodes;
		const EIGEN_VECTOR(Rational) weights = compute_stencil_vandermonde(derivative_order,
		                                                                   center,
		                                                                   nodes);

		assert(nodes.size() == stencil.weights.size());

#if 1
		// I need to compare the reference weights (std::vector<Rational>)
		// with the computed weights (EIGEN_VECTOR(Rational))
		//
		// In order to have structured error messages, I decided
		// it was best to build an EIGEN_VECTOR(Rational) from the original
		// weights
		const auto reference_weights = std_vector_to_eigen(stencil.weights);

		if (weights != reference_weights) {
			ok = false;
			std::cout << "ERROR" << std::endl;
			std::cout << "derivative order = " << derivative_order << std::endl;

			std::cout << "reference weights" << std::endl;
			for (const auto &x : reference_weights) std::cout << x << " ";
			std::cout << std::endl;

			std::cout << "actual weights" << std::endl;
			for (const auto &x : weights) std::cout << x << " ";
			std::cout << std::endl;
		}
#endif
	}

	return 0;
}
