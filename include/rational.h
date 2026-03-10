#ifndef __RATIONAL_H__
#define __RATIONAL_H__


#include <Eigen/Core>
#include <boost/rational.hpp>



// I need the rational type to provide the conversion
// operator to floating point types. This must be
// a **non-static member function**
//
// Two options were considered for integrating boost::rational:
//   1. composition: wrap the boost rational inside a class
//
//   2. inheritance: inherit from boost::rational<IntType> and
//                   add the conversion operators
//
// if I got things right, in any case I would have needed to
// write the arithmetic operators. This needs to be further
// studied because I'm not sure about that. Anyway I chose (1)
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
