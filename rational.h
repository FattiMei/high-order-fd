#ifndef __RATIONAL_H__
#define __RATIONAL_H__


#include <boost/rational.hpp>


// I need rational numbers to behave exactly as primitive floating point types,
// so I'm defining the conversion operator.
//
// Then we need to define a bunch of operator for defining the arithmetic, but
// there is a strong warning
template <typename IntType>
class Rational : public boost::rational<IntType> {
public:
	Rational() : boost::rational<IntType>() {}
	Rational(const boost::rational<IntType>& r) : boost::rational<IntType>(r) {}
	Rational(IntType n) : boost::rational<IntType>(n) {}


	// boost::rational<IntType> doesn't have any explicit
	// casting operators to floating point types, but provides
	// the rational_cast function
	template <typename T>
	explicit operator T() const {
		// rational_cast may lose precision on the inputs
		// where either the numerator or the denominator
		// is not exactly representable in T.
		//
		// this should not be the case for our purposes,
		// but it may be nice to add a check
		return rational_cast<T>(*this);
	}
};


#endif // __RATIONAL_H__
