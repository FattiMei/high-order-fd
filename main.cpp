#include <vector>
#include <stdlib.h>
#include <iostream>
#include "stencil.h"
#include "rational.h"


int main(int argc, char* argv[]) {
	int npoints = 3;

	if (argc > 1) {
		npoints = atoi(argv[1]);
	}

	const auto S = compute_laplacian_stencils<boost::rational<long>>(npoints);

	std::cout << S << std::endl;

	return 0;
}
