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

	// In order to behave like a value type, this can't be const
	boost::rational<IntType> r_;
	Rational(const boost::rational<IntType>& r) : r_(r) {}
};


// I need this function to be declared for Eigen to work,
// but this implementation needs to look at data I wanted to be private...
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
