#include <cassert>
#include <iostream>
#include <boost/rational.hpp>


int main() {
	using rational = boost::rational<int>;

	const rational CONSTANT_A(2,3), CONSTANT_B(7,4);
	rational a(CONSTANT_A), b(CONSTANT_B);

	std::cout << "a = " << a << std::endl;
	std::cout << "b = " << b << std::endl;

	std::cout << "Performing c = a * (-b)" << std::endl;
	rational c = a * (-b);

	std::cout << "a = " << a << std::endl;
	std::cout << "b = " << b << std::endl;
	std::cout << "c = " << c << std::endl;

	assert(a == CONSTANT_A);
	assert(b == CONSTANT_B);

	return 0;
}
