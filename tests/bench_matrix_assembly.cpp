#include <chrono>
#include <cstdlib>
#include <iostream>
#include "solvers/sparse.h"


#define TIC() std::chrono::high_resolution_clock::now()


using Mat = Eigen::MatrixXd;
using SpMat = Eigen::SparseMatrix<double>;


Mat
generate_test_stencil(int stencil_size) {
	Mat res(stencil_size, stencil_size);

	for (int i = 0; i < stencil_size; ++i) {
		for (int j = 0; j < stencil_size; ++j) {
			res(i,j) = i;
		}
	}

	return res;
}


class CompressedSparseMatrix {
public:
	CompressedSparseMatrix(int problem_size, int stencil_size) : nnz(1 + (problem_size-2)*stencil_size + 1) {
		m_data = (double*) malloc(nnz * sizeof(double));
		m_innerIndices = (int*) malloc(nnz * sizeof(int));
		m_outerStarts = (int*) malloc((problem_size+1)* sizeof(int));

		// assemble the matrix along rows
		int write_idx = 0;
		for (int row = 0; row < problem_size; ++row) {
			m_outerStarts[row] = write_idx;

			// I'm confident that the loop peeling will take care of this
			if (row == 0 or row == (problem_size-1)) {
				m_data[write_idx] = 1.0;
				m_innerIndices[write_idx] = 0;
				++write_idx;
			} else {
				// phony writes, only the number of writes is correct
				for (int col = 0; col < stencil_size; ++col) {
					m_data[write_idx] = 1.0;
					m_innerIndices[write_idx] = col;
					++write_idx;
				}
			}
		}

		m_outerStarts[problem_size] = write_idx;
	}

	int get_nnz() const {
		return nnz;
	}

	~CompressedSparseMatrix() {
		free(m_data);
		free(m_outerStarts);
		free(m_innerIndices);
	}

private:
	const int nnz;
	double* m_data;
	int* m_outerStarts;
	int* m_innerIndices;
};


int main(int argc, char* argv[]) {
	int MAX_PROBLEM_SIZE = 20'000;
	if (argc > 1) {
		MAX_PROBLEM_SIZE = std::stoi(argv[1]);
	}

	std::cout << "n,nnz,assembly_time_s,lower_bound_s" << std::endl;

	for (int n = 16; n < MAX_PROBLEM_SIZE; n *= 2) {
		for (int stencil_size = 3; stencil_size <= 9; ++stencil_size) {
			const Mat stencil = generate_test_stencil(stencil_size);

			const auto assembly_start_time = TIC();
			const SpMat A = assemble_system_matrix(n, stencil);
			const auto assembly_stop_time = TIC();

			const std::chrono::duration<double> assembly_time = assembly_stop_time - assembly_start_time;

			const auto phony_assembly_start_time = TIC();
			CompressedSparseMatrix phony(n, stencil_size);
			const auto phony_assembly_stop_time = TIC();
			const std::chrono::duration<double> lower_bound = phony_assembly_stop_time - phony_assembly_start_time;

			std::cout << n
			          << ','
			          << A.nonZeros()
			          << ','
			          << assembly_time.count()
			          << ','
			          << lower_bound.count()
			          << std::endl;
		}
	}

	return 0;
}
