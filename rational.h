#ifndef __RATIONAL_H__
#define __RATIONAL_H__


#include <Eigen/Core>
#include <boost/rational.hpp>


// Rational numbers are required for doing the stencil computations
// in exact arithmetic. Rather than implement them by myself, I
// decided to include the boost dependency.
//
// The implementation must provide pure arithmetic operations such that
// a generic expression `a op b` doesn't mutate any of the variables.
//
// The boost::rational type doesn't explicitly provide pure binary operators
// (+, -, *, /), but only their operator+assignment versions (+=, -=, *=, /=)
// It seems that the C++ compiler is smart enough to fill the gaps
namespace Eigen {
template <typename IntType>
struct NumTraits<boost::rational<IntType>> : GenericNumTraits<boost::rational<IntType>> {
	typedef boost::rational<IntType> Real;
	typedef boost::rational<IntType> NonInteger;

	enum {
		IsComplex = 0,
		IsInteger = 0,
		IsSigned = 1,
		RequireInitialization = 1,
		ReadCost = Eigen::HugeCost,
		AddCost = Eigen::HugeCost,
		MulCost = Eigen::HugeCost
	};

	static inline Real epsilon() { return 0; }
	static inline int digits10() { return 0; }
};

}

#endif // __RATIONAL_H__
