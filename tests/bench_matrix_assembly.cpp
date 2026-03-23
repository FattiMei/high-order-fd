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
	CompressedSparseMatrix(int problem_size, int stencil_size) : nnz(1 + (problem_size-2)*stencil_size + 1),
	                                                             m_problem_size(problem_size) {

		const auto allocation_start_time = TIC();
		m_data = (double*) malloc(nnz * sizeof(double));
		m_innerIndices = (int*) malloc(nnz * sizeof(int));
		m_outerStarts = (int*) malloc((problem_size+1)* sizeof(int));
		const auto allocation_stop_time = TIC();
		m_allocation_time = allocation_stop_time - allocation_start_time;

		// assemble the matrix along rows
		const auto fill_start_time = TIC();
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
		const auto fill_end_time = TIC();
		m_fill_time = fill_end_time - fill_start_time;
	}

	int get_nnz() const {
		return nnz;
	}

	long bytes_written() const {
		return nnz * sizeof(double) + nnz * sizeof(int) + (m_problem_size+1) * sizeof(int);
	}

	std::chrono::duration<double> get_allocation_time() const {
		return m_allocation_time;
	}

	std::chrono::duration<double> get_fill_time() const {
		return m_fill_time;
	}

	~CompressedSparseMatrix() {
		free(m_data);
		free(m_outerStarts);
		free(m_innerIndices);
	}

private:
	const int nnz;
	const int m_problem_size;
	double* m_data;
	int* m_outerStarts;
	int* m_innerIndices;

	std::chrono::duration<double> m_allocation_time;
	std::chrono::duration<double> m_fill_time;
};


int main(int argc, char* argv[]) {
	int MAX_PROBLEM_SIZE = 20'000;
	if (argc > 1) {
		MAX_PROBLEM_SIZE = std::stoi(argv[1]);
	}

	std::cout << "n,stencil_size,nnz,assembly_time_s,lower_bound_s,fill_time_s,bytes_written" << std::endl;

	for (int n = 16; n < MAX_PROBLEM_SIZE; n *= 2) {
		for (int stencil_size = 3; stencil_size <= 9; ++stencil_size) {
			const Mat stencil = generate_test_stencil(stencil_size);

			const auto assembly_start_time = TIC();
			const SpMat A = assemble_system_matrix(n, stencil);
			const auto assembly_stop_time = TIC();

			const std::chrono::duration<double> assembly_time = assembly_stop_time - assembly_start_time;

			CompressedSparseMatrix phony(n, stencil_size);
			const std::chrono::duration<double> best_bound = phony.get_allocation_time() + phony.get_fill_time();

			std::cout << n
			          << ','
			          << stencil_size
			          << ','
			          << A.nonZeros()
			          << ','
			          << assembly_time.count()
			          << ','
			          << best_bound.count()
			          << ','
			          << phony.get_fill_time().count()
			          << ','
			          << phony.bytes_written()
			          << std::endl;
		}
	}

	return 0;
}
