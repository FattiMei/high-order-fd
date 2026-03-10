#ifndef __RATIONAL_H__
#define __RATIONAL_H__


#include <Eigen/Core>
#include <boost/rational.hpp>



// I need the rational type to provide the conversion
// operator to floating point types. This must be
// a **non-static member function**
template <typename IntType>
class rational {
public:
	boost::rational<IntType> m_value;

	rational() : m_value() {}
	rational(IntType n) : m_value(n) {}
	rational(boost::rational<IntType> &r) : m_value(r) {}

	rational<IntType> operator-() const {
		auto tmp = m_value;
		tmp = -tmp;

		return rational(tmp);
	}

	rational<IntType> operator+(const rational<IntType>& r) const {
		auto tmp = m_value;
		tmp += r.m_value;

		return rational(tmp);
	}

	rational<IntType> operator-(const rational<IntType>& r) const {
		auto tmp = m_value;
		tmp -= r.m_value;

		return rational(tmp);
	}

	rational<IntType> operator*(const rational<IntType>& r) const {
		auto tmp = m_value;
		tmp *= r.m_value;

		return rational(tmp);
	}

	rational<IntType> operator+=(const rational<IntType>& r) {
		m_value += r.m_value;

		return *this;
	}

	rational<IntType> operator-=(const rational<IntType>& r) {
		m_value -= r.m_value;

		return *this;
	}

	rational<IntType> operator*=(const rational<IntType>& r) {
		m_value *= r.m_value;

		return *this;
	}

	rational<IntType> operator/=(const rational<IntType>& r) {
		m_value /= r.m_value;

		return *this;
	}

	bool operator==(const rational<IntType> &r) const {
		return m_value == r.m_value;
	}

	bool operator<(const rational<IntType> &r) const {
		return m_value < r.m_value;
	}

	bool operator>(const rational<IntType> &r) const {
		return m_value > r.m_value;
	}

	template <typename FloatType>
	operator FloatType() const {
		return boost::rational_cast<FloatType>(m_value);
	}
};


template <typename IntType>
rational<IntType> abs(const rational<IntType>& r) {
	auto tmp = abs(r.m_value);

	return rational<IntType>(tmp);
}


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
struct NumTraits<rational<IntType>> : GenericNumTraits<rational<IntType>> {
	typedef rational<IntType> Real;
	typedef rational<IntType> NonInteger;

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
