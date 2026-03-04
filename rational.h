#ifndef __RATIONAL_H__
#define __RATIONAL_H__


#include <ostream>
#include <boost/rational.hpp>


// Rational numbers are required for doing the stencil computations
// in exact arithmetic. Rather than implement them by myself, I
// decided to include the boost dependency.
//
// The implementation must provide pure arithmetic operations such that
// a generic expression `a op b` doesn't mutate any of the variables.
// This is critical because I have third party code (Eigen) that uses this
// scalar type and I can't make any assumptions
//
// I decided to wrap the `boost::rational<IntType>` class instead of inherit
// from so that I control every behaviour
template <typename IntType>
class Rational {
public:
	Rational() : r_() {}
	Rational(int n) : r_(n) {}

	Rational operator-() const {
		boost::rational<IntType> tmp = r_;
		tmp = -tmp;

		return Rational(tmp);
	}

	// most of the arithmetic operators implemented in
	// boost::rational mutate the first operand!
	Rational operator+(const Rational& other) const {
		boost::rational<IntType> tmp = r_;
		tmp += other.r_;

		return Rational(tmp);
	}

	Rational operator*(const Rational& other) const {
		boost::rational<IntType> tmp = r_;
		tmp *= other.r_;

		return Rational(tmp);
	}

	// in-place operators
	Rational& operator+=(const Rational& other) {
		r_ += other.r_;

		return *this;
	}

	Rational& operator-=(const Rational& other) {
		r_ -= other.r_;

		return *this;
	}

	Rational& operator*=(const Rational& other) {
		r_ *= other.r_;

		return *this;
	}

	Rational& operator/=(const Rational& other) {
		r_ /= other.r_;

		return *this;
	}

	// comparison operators
	bool operator==(const Rational& q) const { return r_ == q.r_; }
	bool operator> (const Rational& q) const { return r_  > q.r_; }
	bool operator< (const Rational& q) const { return r_  < q.r_; }

	// conversion operators
	// this may be templated on the `std::floating_point` concept
	explicit operator double() const {
		return rational_cast<double>(r_);
	}

	// can we make `r_` private? I know the `abs` function needs to access it
	// but I don't want users of Rational to inspect its internal state
	boost::rational<IntType> r_;
	Rational(const boost::rational<IntType>& r) : r_(r) {}
};


// I wanted to completely hide to the user boost::rational, but it seems impossible...
// I think the problem lies in the templating: if the user wants a Rational<UserType>,
// then he must be able to see boost::rational<UserType>.
template <typename IntType>
Rational<IntType> abs(const Rational<IntType>& r) {
	// in the absence of move constructors this line wouldn't work
	// return Rational<IntType>(abs(r.r_));

	const auto tmp = abs(r.r_);

	return Rational<IntType>(tmp);
}


template <typename IntType>
std::ostream& operator<<(std::ostream& out, const Rational<IntType>& r) {
	return out << r.r_;
}


#endif // __RATIONAL_H__
