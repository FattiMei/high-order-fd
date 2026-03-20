#include <chrono>
#include <cstdlib>
#include <iostream>


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


#define TIC() std::chrono::high_resolution_clock::now()


int main(int argc, char* argv[]) {
	int MAX_PROBLEM_SIZE = 20'000;
	if (argc > 1) {
		MAX_PROBLEM_SIZE = std::stoi(argv[1]);
	}

	std::cout << "n,name,nnz,assemble_time_s" << std::endl;

	for (int n = 16; n < MAX_PROBLEM_SIZE; n *= 2) {
		#define RUN_ASSEMBLY(stencil_size, name) do {                                                        \
			const auto assemble_start_time = TIC();                                                      \
			auto assembler = CompressedSparseMatrix(n, stencil_size);                                    \
			const auto assemble_end_time = TIC();                                                        \
			                                                                                             \
			const std::chrono::duration<double> assemble_time = assemble_end_time - assemble_start_time; \
			                                                                                             \
			std::cout << n                                                                               \
				  << ','                                                                             \
				  << name                                                                            \
				  << ','                                                                             \
				  << assembler.get_nnz()                                                             \
				  << ','                                                                             \
				  << assemble_time.count()                                                           \
				  << std::endl;                                                                      \
		} while(0)

		RUN_ASSEMBLY(3, "3-point");
		RUN_ASSEMBLY(5, "5-point");
		RUN_ASSEMBLY(7, "7-point");
		RUN_ASSEMBLY(9, "9-point");
	}

	return 0;
}
