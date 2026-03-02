#include <vector>
#include <iostream>
#include "stencil.h"


int main() {
	std::vector<int> nodes{0, 1, 2};
	const auto A = compute_stencil_vandermonde(0.0, 2, nodes);
	const auto S = compute_laplacian_stencils<float>(3);

	return 0;
}
